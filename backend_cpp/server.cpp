#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cctype>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "AVLTree.h"

#define BUFFER_SIZE 65536
#define DEFAULT_PORT 8000

class CppWebServer {
private:
    int port;
    SOCKET listenSocket;
    AVLTree tree;
    std::string frontendPath;

    std::string getContentType(const std::string& path) {
        if (path.rfind(".html") != std::string::npos) return "text/html; charset=UTF-8";
        if (path.rfind(".css") != std::string::npos) return "text/css; charset=UTF-8";
        if (path.rfind(".js") != std::string::npos) return "application/javascript; charset=UTF-8";
        if (path.rfind(".json") != std::string::npos) return "application/json; charset=UTF-8";
        if (path.rfind(".svg") != std::string::npos) return "image/svg+xml";
        if (path.rfind(".zip") != std::string::npos) return "application/zip";
        if (path.rfind(".pdf") != std::string::npos) return "application/pdf";
        return "text/plain; charset=UTF-8";
    }

    void sendResponse(SOCKET clientSocket, int statusCode, const std::string& statusText,
                      const std::string& contentType, const std::string& body) {
        std::stringstream ss;
        ss << "HTTP/1.1 " << statusCode << " " << statusText << "\r\n";
        ss << "Content-Type: " << contentType << "\r\n";
        ss << "Content-Length: " << body.size() << "\r\n";
        ss << "Access-Control-Allow-Origin: *\r\n";
        ss << "Access-Control-Allow-Methods: GET, POST, DELETE, OPTIONS\r\n";
        ss << "Access-Control-Allow-Headers: Content-Type\r\n";
        ss << "Connection: close\r\n\r\n";
        ss << body;

        std::string res = ss.str();
        send(clientSocket, res.c_str(), (int)res.size(), 0);
    }

    void sendBinaryFile(SOCKET clientSocket, const std::string& filePath, const std::string& contentType) {
        std::ifstream file(filePath.c_str(), std::ios::binary);
        if (!file.is_open()) {
            sendResponse(clientSocket, 404, "Not Found", "application/json", "{\"detail\":\"File not found\"}");
            return;
        }

        file.seekg(0, std::ios::end);
        size_t fileSize = file.tellg();
        file.seekg(0, std::ios::beg);

        std::stringstream header;
        header << "HTTP/1.1 200 OK\r\n";
        header << "Content-Type: " << contentType << "\r\n";
        header << "Content-Length: " << fileSize << "\r\n";
        header << "Access-Control-Allow-Origin: *\r\n";
        header << "Connection: close\r\n\r\n";

        std::string headerStr = header.str();
        send(clientSocket, headerStr.c_str(), (int)headerStr.size(), 0);

        char buffer[8192];
        while (file.read(buffer, sizeof(buffer)) || file.gcount() > 0) {
            send(clientSocket, buffer, (int)file.gcount(), 0);
        }
        file.close();
    }

    int extractInt(const std::string& json, const std::string& key) {
        size_t pos = json.find("\"" + key + "\"");
        if (pos == std::string::npos) return 0;
        pos = json.find(':', pos);
        if (pos == std::string::npos) return 0;
        pos++;
        while (pos < json.size() && (json[pos] == ' ' || json[pos] == '\t' || json[pos] == '\"')) pos++;
        size_t end = pos;
        while (end < json.size() && (isdigit(json[end]) || json[end] == '-')) end++;
        try {
            return std::stoi(json.substr(pos, end - pos));
        } catch (...) { return 0; }
    }

    std::string extractString(const std::string& json, const std::string& key) {
        size_t pos = json.find("\"" + key + "\"");
        if (pos == std::string::npos) return "";
        pos = json.find(':', pos);
        if (pos == std::string::npos) return "";
        pos = json.find('\"', pos);
        if (pos == std::string::npos) return "";
        pos++;
        size_t end = json.find('\"', pos);
        if (end == std::string::npos) return "";
        return json.substr(pos, end - pos);
    }

    double extractDouble(const std::string& json, const std::string& key) {
        size_t pos = json.find("\"" + key + "\"");
        if (pos == std::string::npos) return -1.0;
        pos = json.find(':', pos);
        if (pos == std::string::npos) return -1.0;
        pos++;
        while (pos < json.size() && (json[pos] == ' ' || json[pos] == '\t' || json[pos] == '\"')) pos++;
        size_t end = pos;
        while (end < json.size() && (isdigit(json[end]) || json[end] == '.' || json[end] == '-')) end++;
        try {
            return std::stod(json.substr(pos, end - pos));
        } catch (...) { return -1.0; }
    }

    std::string formatRotationsJson(const std::vector<std::string>& rotations) {
        std::stringstream ss;
        ss << "[";
        for (size_t i = 0; i < rotations.size(); ++i) {
            ss << "\"" << rotations[i] << "\"";
            if (i + 1 < rotations.size()) ss << ",";
        }
        ss << "]";
        return ss.str();
    }

public:
    CppWebServer(int port, const std::string& frontendPath)
        : port(port), listenSocket(INVALID_SOCKET), frontendPath(frontendPath) {
        
        // Seed initial student records in C++ AVL Tree
        tree.insert(101, "Alex Rivera", 88.5);
        tree.insert(102, "Maya Chen", 94.0);
        tree.insert(103, "Liam Vance", 79.5);
    }

    ~CppWebServer() {
        if (listenSocket != INVALID_SOCKET) {
            closesocket(listenSocket);
        }
        WSACleanup();
    }

    bool start() {
        WSADATA wsaData;
        int res = WSAStartup(MAKEWORD(2, 2), &wsaData);
        if (res != 0) {
            std::cerr << "[ERROR] WSAStartup failed: " << res << std::endl;
            return false;
        }

        listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (listenSocket == INVALID_SOCKET) {
            std::cerr << "[ERROR] Socket creation failed: " << WSAGetLastError() << std::endl;
            WSACleanup();
            return false;
        }

        char opt = 1;
        setsockopt(listenSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

        sockaddr_in serverAddr;
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_addr.s_addr = INADDR_ANY;
        serverAddr.sin_port = htons(port);

        if (bind(listenSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
            std::cerr << "[ERROR] Bind failed on port " << port << ": " << WSAGetLastError() << std::endl;
            closesocket(listenSocket);
            WSACleanup();
            return false;
        }

        if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR) {
            std::cerr << "[ERROR] Listen failed: " << WSAGetLastError() << std::endl;
            closesocket(listenSocket);
            WSACleanup();
            return false;
        }

        std::cout << "==========================================================" << std::endl;
        std::cout << "  TreeVault C++ Native Backend Server is ACTIVE!" << std::endl;
        std::cout << "  Running 100% on C++ AVL Tree Engine (Port " << port << ")" << std::endl;
        std::cout << "  Local Website URL: http://localhost:" << port << std::endl;
        std::cout << "==========================================================" << std::endl;

        return true;
    }

    void handleClient(SOCKET clientSocket) {
        char buffer[BUFFER_SIZE];
        int bytesReceived = recv(clientSocket, buffer, BUFFER_SIZE - 1, 0);
        if (bytesReceived <= 0) {
            closesocket(clientSocket);
            return;
        }
        buffer[bytesReceived] = '\0';

        std::string request(buffer, bytesReceived);
        std::istringstream reqStream(request);
        std::string method, path, protocol;
        reqStream >> method >> path >> protocol;

        size_t queryPos = path.find('?');
        if (queryPos != std::string::npos) {
            path = path.substr(0, queryPos);
        }

        if (method == "OPTIONS") {
            sendResponse(clientSocket, 204, "No Content", "text/plain", "");
            closesocket(clientSocket);
            return;
        }

        // ---------------- API ROUTES ----------------

        // 1. GET /api/tree
        if (method == "GET" && path == "/api/tree") {
            sendResponse(clientSocket, 200, "OK", "application/json; charset=UTF-8", tree.toJSON());
        }
        // 2. GET /api/tree/stats
        else if (method == "GET" && path == "/api/tree/stats") {
            std::stringstream ss;
            ss << "{\"count\":" << tree.size()
               << ",\"height\":" << tree.getHeight()
               << ",\"is_balanced\":" << (tree.validate() ? "true" : "false")
               << ",\"total_rotations\":" << tree.getTotalRotations() << "}";
            sendResponse(clientSocket, 200, "OK", "application/json; charset=UTF-8", ss.str());
        }
        // 3. GET /api/tree/traversal/{order}
        else if (method == "GET" && path.rfind("/api/tree/traversal/", 0) == 0) {
            std::string order = path.substr(std::string("/api/tree/traversal/").length());
            std::vector<Student> list;
            if (order == "inorder") list = tree.inorder();
            else if (order == "preorder") list = tree.preorder();
            else list = tree.postorder();

            std::stringstream ss;
            ss << "{\"order\":\"" << order << "\",\"students\":[";
            for (size_t i = 0; i < list.size(); ++i) {
                ss << "{\"id\":" << list[i].id
                   << ",\"name\":\"" << list[i].name << "\""
                   << ",\"marks\":" << list[i].marks << "}";
                if (i + 1 < list.size()) ss << ",";
            }
            ss << "]}";
            sendResponse(clientSocket, 200, "OK", "application/json; charset=UTF-8", ss.str());
        }
        // 4. GET /api/students
        else if (method == "GET" && path == "/api/students") {
            std::vector<Student> list = tree.inorder();
            std::stringstream ss;
            ss << "[";
            for (size_t i = 0; i < list.size(); ++i) {
                ss << "{\"id\":" << list[i].id
                   << ",\"name\":\"" << list[i].name << "\""
                   << ",\"marks\":" << list[i].marks << "}";
                if (i + 1 < list.size()) ss << ",";
            }
            ss << "]";
            sendResponse(clientSocket, 200, "OK", "application/json; charset=UTF-8", ss.str());
        }
        // 5. POST /api/students
        else if (method == "POST" && path == "/api/students") {
            size_t bodyPos = request.find("\r\n\r\n");
            std::string body = (bodyPos != std::string::npos) ? request.substr(bodyPos + 4) : "";

            int id = extractInt(body, "id");
            std::string name = extractString(body, "name");
            double marks = extractDouble(body, "marks");

            if (id <= 0 || name.empty() || marks < 0 || marks > 100) {
                sendResponse(clientSocket, 400, "Bad Request", "application/json; charset=UTF-8",
                             "{\"detail\":\"Invalid student data. ID must be > 0 and marks between 0-100.\"}");
            } else {
                OperationResult res = tree.insert(id, name, marks);
                if (res.success) {
                    std::stringstream ss;
                    ss << "{\"success\":true,\"message\":\"" << res.message << "\""
                       << ",\"student\":{\"id\":" << id << ",\"name\":\"" << name << "\",\"marks\":" << marks << "}"
                       << ",\"rotations\":" << formatRotationsJson(res.rotations)
                       << ",\"tree\":" << tree.toJSON() << "}";
                    sendResponse(clientSocket, 200, "OK", "application/json; charset=UTF-8", ss.str());
                } else {
                    sendResponse(clientSocket, 409, "Conflict", "application/json; charset=UTF-8",
                                 "{\"detail\":\"" + res.message + "\"}");
                }
            }
        }
        // 6. GET /api/students/{id} or DELETE /api/students/{id}
        else if (path.rfind("/api/students/", 0) == 0) {
            std::string idStr = path.substr(std::string("/api/students/").length());
            try {
                int id = std::stoi(idStr);
                if (method == "GET") {
                    Student s;
                    std::vector<int> searchPath;
                    if (tree.search(id, s, searchPath)) {
                        std::stringstream ss;
                        ss << "{\"found\":true,\"student\":{\"id\":" << s.id
                           << ",\"name\":\"" << s.name << "\",\"marks\":" << s.marks << "}"
                           << ",\"search_path\":[";
                        for (size_t i = 0; i < searchPath.size(); ++i) {
                            ss << searchPath[i];
                            if (i + 1 < searchPath.size()) ss << ",";
                        }
                        ss << "]}";
                        sendResponse(clientSocket, 200, "OK", "application/json; charset=UTF-8", ss.str());
                    } else {
                        sendResponse(clientSocket, 404, "Not Found", "application/json; charset=UTF-8",
                                     "{\"detail\":\"Student not found in AVL Tree.\"}");
                    }
                } else if (method == "DELETE") {
                    OperationResult res = tree.deleteStudent(id);
                    if (res.success) {
                        std::stringstream ss;
                        ss << "{\"success\":true,\"message\":\"" << res.message << "\""
                           << ",\"rotations\":" << formatRotationsJson(res.rotations)
                           << ",\"tree\":" << tree.toJSON() << "}";
                        sendResponse(clientSocket, 200, "OK", "application/json; charset=UTF-8", ss.str());
                    } else {
                        sendResponse(clientSocket, 404, "Not Found", "application/json; charset=UTF-8",
                                     "{\"detail\":\"" + res.message + "\"}");
                    }
                }
            } catch (...) {
                sendResponse(clientSocket, 400, "Bad Request", "application/json; charset=UTF-8",
                             "{\"detail\":\"Invalid student ID parameter.\"}");
            }
        }
        // 7. DELETE /api/clear
        else if (method == "DELETE" && path == "/api/clear") {
            tree.clear();
            sendResponse(clientSocket, 200, "OK", "application/json; charset=UTF-8",
                         "{\"message\":\"AVL Tree cleared successfully.\"}");
        }
        // ---------------- STATIC FILE SERVING ----------------
        else if (method == "GET") {
            std::string filePath = frontendPath;
            if (path == "/" || path.empty()) {
                filePath += "/index.html";
            } else {
                filePath += path;
            }

            std::string cType = getContentType(filePath);
            sendBinaryFile(clientSocket, filePath, cType);
        } else {
            sendResponse(clientSocket, 405, "Method Not Allowed", "text/plain", "Method Not Allowed");
        }

        closesocket(clientSocket);
    }

    void run() {
        while (true) {
            sockaddr_in clientAddr;
            int clientLen = sizeof(clientAddr);
            SOCKET clientSocket = accept(listenSocket, (sockaddr*)&clientAddr, &clientLen);
            if (clientSocket == INVALID_SOCKET) {
                std::cerr << "[WARN] Accept failed: " << WSAGetLastError() << std::endl;
                continue;
            }
            handleClient(clientSocket);
        }
    }
};

int main(int argc, char* argv[]) {
    int port = DEFAULT_PORT;
    if (argc > 1) {
        port = std::stoi(argv[1]);
    }

    std::string frontendPath = "../frontend";
    std::ifstream testF("frontend/index.html");
    if (testF.good()) {
        frontendPath = "frontend";
    }

    CppWebServer server(port, frontendPath);
    if (!server.start()) {
        return 1;
    }

    server.run();
    return 0;
}
