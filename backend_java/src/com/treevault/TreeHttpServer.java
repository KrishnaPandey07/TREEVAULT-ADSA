package com.treevault;

import com.sun.net.httpserver.HttpExchange;
import com.sun.net.httpserver.HttpHandler;
import com.sun.net.httpserver.HttpServer;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.net.InetSocketAddress;
import java.nio.charset.StandardCharsets;
import java.util.ArrayList;
import java.util.List;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

/**
 * Standalone Java Full-Stack HTTP Web Server for TreeVault.
 * Built with Java's standard library com.sun.net.httpserver (zero external dependencies).
 * Serves both the REST API and the Frontend website!
 */
public class TreeHttpServer {

    private static final int PORT = 8080;
    private static final AVLTree tree = new AVLTree();
    private static String frontendDir = "frontend";

    public static void main(String[] args) throws IOException {
        // Find frontend directory
        if (!new File("frontend").exists() && new File("../frontend").exists()) {
            frontendDir = "../frontend";
        }

        // Seed demo data
        tree.insert(101, "Alex Rivera", 88.5);
        tree.insert(102, "Maya Chen", 94.0);
        tree.insert(103, "Liam Vance", 79.5);

        HttpServer server = HttpServer.create(new InetSocketAddress(PORT), 0);

        // API contexts
        server.createContext("/api/tree", new TreeHandler());
        server.createContext("/api/students", new StudentsHandler());
        server.createContext("/api/clear", new ClearHandler());

        // Static file context (serves HTML/CSS/JS frontend)
        server.createContext("/", new StaticFileHandler());

        server.setExecutor(null);
        System.out.println("==================================================");
        System.out.println("  TreeVault Java Native Web Server is ACTIVE!");
        System.out.println("  Running on: http://localhost:" + PORT);
        System.out.println("==================================================");
        server.start();
    }

    private static void sendResponse(HttpExchange exchange, int statusCode, String response) throws IOException {
        byte[] bytes = response.getBytes(StandardCharsets.UTF_8);
        exchange.getResponseHeaders().set("Content-Type", "application/json; charset=UTF-8");
        exchange.getResponseHeaders().set("Access-Control-Allow-Origin", "*");
        exchange.getResponseHeaders().set("Access-Control-Allow-Methods", "GET, POST, DELETE, OPTIONS");
        exchange.getResponseHeaders().set("Access-Control-Allow-Headers", "Content-Type");

        exchange.sendResponseHeaders(statusCode, bytes.length);
        OutputStream os = exchange.getResponseBody();
        os.write(bytes);
        os.close();
    }

    static class StaticFileHandler implements HttpHandler {
        @Override
        public void handle(HttpExchange exchange) throws IOException {
            String path = exchange.getRequestURI().getPath();
            if (path.startsWith("/api/")) {
                sendResponse(exchange, 404, "{\"detail\":\"API endpoint not found\"}");
                return;
            }

            if (path.equals("/") || path.isEmpty()) {
                path = "/index.html";
            }

            File file = new File(frontendDir + path);
            if (!file.exists() || file.isDirectory()) {
                sendResponse(exchange, 404, "{\"detail\":\"File not found\"}");
                return;
            }

            String contentType = "text/plain";
            if (path.endsWith(".html")) contentType = "text/html; charset=UTF-8";
            else if (path.endsWith(".css")) contentType = "text/css; charset=UTF-8";
            else if (path.endsWith(".js")) contentType = "application/javascript; charset=UTF-8";
            else if (path.endsWith(".svg")) contentType = "image/svg+xml";
            else if (path.endsWith(".zip")) contentType = "application/zip";

            exchange.getResponseHeaders().set("Content-Type", contentType);
            exchange.getResponseHeaders().set("Access-Control-Allow-Origin", "*");
            exchange.sendResponseHeaders(200, file.length());

            try (FileInputStream fis = new FileInputStream(file); OutputStream os = exchange.getResponseBody()) {
                byte[] buffer = new byte[8192];
                int bytesRead;
                while ((bytesRead = fis.read(buffer)) != -1) {
                    os.write(buffer, 0, bytesRead);
                }
            }
        }
    }

    static class TreeHandler implements HttpHandler {
        @Override
        public void handle(HttpExchange exchange) throws IOException {
            String path = exchange.getRequestURI().getPath();
            String method = exchange.getRequestMethod();

            if ("OPTIONS".equalsIgnoreCase(method)) {
                sendResponse(exchange, 204, "");
                return;
            }

            if (path.equals("/api/tree/stats")) {
                String json = "{\"count\":" + tree.size()
                        + ",\"height\":" + tree.getHeight()
                        + ",\"is_balanced\":" + tree.validate()
                        + ",\"total_rotations\":" + tree.getTotalRotations() + "}";
                sendResponse(exchange, 200, json);
            } else if (path.startsWith("/api/tree/traversal/")) {
                String order = path.substring("/api/tree/traversal/".length());
                List<Student> list;
                if ("inorder".equalsIgnoreCase(order)) list = tree.inorder();
                else if ("preorder".equalsIgnoreCase(order)) list = tree.preorder();
                else list = tree.postorder();

                StringBuilder sb = new StringBuilder("{\"order\":\"" + order + "\",\"students\":[");
                for (int i = 0; i < list.size(); i++) {
                    Student s = list.get(i);
                    sb.append("{\"id\":").append(s.getId())
                      .append(",\"name\":\"").append(s.getName())
                      .append("\",\"marks\":").append(s.getMarks()).append("}");
                    if (i + 1 < list.size()) sb.append(",");
                }
                sb.append("]}");
                sendResponse(exchange, 200, sb.toString());
            } else {
                sendResponse(exchange, 200, tree.toJSON());
            }
        }
    }

    static class StudentsHandler implements HttpHandler {
        @Override
        public void handle(HttpExchange exchange) throws IOException {
            String method = exchange.getRequestMethod();
            String path = exchange.getRequestURI().getPath();

            if ("OPTIONS".equalsIgnoreCase(method)) {
                sendResponse(exchange, 204, "");
                return;
            }

            Pattern pattern = Pattern.compile("^/api/students/(\\d+)$");
            Matcher matcher = pattern.matcher(path);

            if (matcher.matches()) {
                int id = Integer.parseInt(matcher.group(1));

                if ("GET".equalsIgnoreCase(method)) {
                    Student[] s = new Student[1];
                    List<Integer> searchPath = new ArrayList<>();
                    if (tree.search(id, s, searchPath)) {
                        String json = "{\"found\":true,\"student\":{\"id\":" + s[0].getId()
                                + ",\"name\":\"" + s[0].getName() + "\",\"marks\":" + s[0].getMarks() + "}"
                                + ",\"search_path\":" + searchPath + "}";
                        sendResponse(exchange, 200, json);
                    } else {
                        sendResponse(exchange, 404, "{\"detail\":\"Student #" + id + " not found.\"}");
                    }
                } else if ("DELETE".equalsIgnoreCase(method)) {
                    AVLTree.OperationResult res = tree.deleteStudent(id);
                    if (res.success) {
                        String rotJson = formatRotations(res.rotations);
                        String json = "{\"success\":true,\"message\":\"" + res.message
                                + "\",\"rotations\":" + rotJson
                                + ",\"tree\":" + tree.toJSON() + "}";
                        sendResponse(exchange, 200, json);
                    } else {
                        sendResponse(exchange, 404, "{\"detail\":\"" + res.message + "\"}");
                    }
                }
                return;
            }

            if ("GET".equalsIgnoreCase(method)) {
                List<Student> list = tree.inorder();
                StringBuilder sb = new StringBuilder("[");
                for (int i = 0; i < list.size(); i++) {
                    Student s = list.get(i);
                    sb.append("{\"id\":").append(s.getId())
                      .append(",\"name\":\"").append(s.getName())
                      .append("\",\"marks\":").append(s.getMarks()).append("}");
                    if (i + 1 < list.size()) sb.append(",");
                }
                sb.append("]");
                sendResponse(exchange, 200, sb.toString());
            } else if ("POST".equalsIgnoreCase(method)) {
                InputStreamReader isr = new InputStreamReader(exchange.getRequestBody(), StandardCharsets.UTF_8);
                BufferedReader br = new BufferedReader(isr);
                StringBuilder body = new StringBuilder();
                String line;
                while ((line = br.readLine()) != null) body.append(line);

                int id = extractInt(body.toString(), "id");
                String name = extractString(body.toString(), "name");
                double marks = extractDouble(body.toString(), "marks");

                if (id <= 0 || name.isEmpty() || marks < 0 || marks > 100) {
                    sendResponse(exchange, 400, "{\"detail\":\"Invalid student data provided.\"}");
                    return;
                }

                AVLTree.OperationResult res = tree.insert(id, name, marks);
                if (res.success) {
                    String rotJson = formatRotations(res.rotations);
                    String json = "{\"success\":true,\"message\":\"" + res.message
                            + "\",\"student\":{\"id\":" + id + ",\"name\":\"" + name + "\",\"marks\":" + marks + "}"
                            + ",\"rotations\":" + rotJson
                            + ",\"tree\":" + tree.toJSON() + "}";
                    sendResponse(exchange, 200, json);
                } else {
                    sendResponse(exchange, 409, "{\"detail\":\"" + res.message + "\"}");
                }
            }
        }

        private static String formatRotations(List<String> list) {
            StringBuilder sb = new StringBuilder("[");
            for (int i = 0; i < list.size(); i++) {
                sb.append("\"").append(list.get(i)).append("\"");
                if (i + 1 < list.size()) sb.append(",");
            }
            sb.append("]");
            return sb.toString();
        }

        private static int extractInt(String json, String key) {
            Matcher m = Pattern.compile("\"" + key + "\"\\s*:\\s*(\\d+)").matcher(json);
            return m.find() ? Integer.parseInt(m.group(1)) : 0;
        }

        private static String extractString(String json, String key) {
            Matcher m = Pattern.compile("\"" + key + "\"\\s*:\\s*\"([^\"]+)\"").matcher(json);
            return m.find() ? m.group(1) : "";
        }

        private static double extractDouble(String json, String key) {
            Matcher m = Pattern.compile("\"" + key + "\"\\s*:\\s*([0-9.]+)").matcher(json);
            return m.find() ? Double.parseDouble(m.group(1)) : 0.0;
        }
    }

    static class ClearHandler implements HttpHandler {
        @Override
        public void handle(HttpExchange exchange) throws IOException {
            if ("DELETE".equalsIgnoreCase(exchange.getRequestMethod())) {
                tree.clear();
                sendResponse(exchange, 200, "{\"message\":\"Tree cleared successfully.\"}");
            } else {
                sendResponse(exchange, 405, "{\"detail\":\"Method not allowed.\"}");
            }
        }
    }
}
