#include "libs/httplib.h"
#include "GameManager.h"
#include "libs/json.hpp" 
#include <iostream>
#include <fstream>
#include <sstream>

using namespace httplib;
using json = nlohmann::json;

// Global Game Manager
GameManager gameManager;

std::string readFile(const std::string& path) {
    std::ifstream f(path);
    if (!f.is_open()) {
        std::string parentPath = "../" + path;
        std::ifstream f2(parentPath);
        if(!f2.is_open()) return "";
        std::stringstream buffer;
        buffer << f2.rdbuf();
        return buffer.str();
    }
    std::stringstream buffer;
    buffer << f.rdbuf();
    return buffer.str();
}

void add_cors_headers(Response& res) {
    res.set_header("Access-Control-Allow-Origin", "*");
    res.set_header("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
    res.set_header("Access-Control-Allow-Headers", "Content-Type");
}

int main() {
    Server svr;

    // Serve Static Files
    svr.Get("/", [](const Request& req, Response& res) {
        std::string content = readFile("web/index.html");
        res.set_content(content, "text/html");
    });
    
    svr.Get("/style.css", [](const Request& req, Response& res) {
        std::string content = readFile("web/style.css");
        res.set_content(content, "text/css");
    });
    
    svr.Get("/script.js", [](const Request& req, Response& res) {
        std::string content = readFile("web/script.js");
        res.set_content(content, "application/javascript");
    });

    // API V1: Create Game
    svr.Post("/api/v1/game/create", [](const Request& req, Response& res) {
        add_cors_headers(res);
        std::string gameId = gameManager.createGame();
        
        json response;
        response["status"] = "success";
        response["data"] = { {"gameId", gameId} };
        res.set_content(response.dump(), "application/json");
    });

    // API V1: Get State
    // URL: /api/v1/game/:gameId/state
    svr.Get(R"(/api/v1/game/([^/]+)/state)", [](const Request& req, Response& res) {
        add_cors_headers(res);
        std::string gameId = req.matches[1];
        auto game = gameManager.getGame(gameId);
        
        if (!game) {
            res.status = 404;
            json response = {{"status", "error"}, {"message", "Game not found"}};
            res.set_content(response.dump(), "application/json");
            return;
        }

        json j = game->getGameState();
        json response;
        response["status"] = "success";
        response["data"] = j;
        res.set_content(response.dump(), "application/json");
    });

    // API V1: Roll Dice
    // URL: /api/v1/game/:gameId/roll
    svr.Post(R"(/api/v1/game/([^/]+)/roll)", [](const Request& req, Response& res) {
        add_cors_headers(res);
        std::string gameId = req.matches[1];
        auto game = gameManager.getGame(gameId);
        
        if (!game) {
            res.status = 404;
            json response = {{"status", "error"}, {"message", "Game not found"}};
            res.set_content(response.dump(), "application/json");
            return;
        }

        try {
            auto j = json::parse(req.body);
            int pId = j["playerId"];
            int roll = game->rollDiceForPlayer(pId);
            
            json response;
            if (roll == -1) {
                response["status"] = "error";
                response["message"] = "Not your turn";
            } else if (roll == -2) {
                response["status"] = "error";
                response["message"] = "Already rolled, waiting for move";
            } else {
                response["status"] = "success";
                response["data"] = { {"roll", roll} };
            }
            res.set_content(response.dump(), "application/json");
        } catch (...) {
            res.status = 400;
        }
    });

    // API V1: Move Piece
    // URL: /api/v1/game/:gameId/move
    svr.Post(R"(/api/v1/game/([^/]+)/move)", [](const Request& req, Response& res) {
        add_cors_headers(res);
        std::string gameId = req.matches[1];
        auto game = gameManager.getGame(gameId);
        
        if (!game) {
            res.status = 404;
            json response = {{"status", "error"}, {"message", "Game not found"}};
            res.set_content(response.dump(), "application/json");
            return;
        }

        try {
            auto j = json::parse(req.body);
            int pId = j["playerId"];
            int pieceIdx = j["pieceId"];
            bool success = game->makeMoveForPlayer(pId, pieceIdx);
            
            json response;
            if (success) {
                 response["status"] = "success";
                 response["data"] = { {"moved", true} };
            } else {
                 response["status"] = "error";
                 response["message"] = "Invalid move";
            }
            res.set_content(response.dump(), "application/json");
        } catch (...) {
            res.status = 400;
        }
    });
    
    // API V1: Reset
    svr.Post(R"(/api/v1/game/([^/]+)/reset)", [](const Request& req, Response& res) {
        add_cors_headers(res);
        std::string gameId = req.matches[1];
        auto game = gameManager.getGame(gameId);
        
        if (!game) {
             res.status = 404;
             json response = {{"status", "error"}, {"message", "Game not found"}};
             res.set_content(response.dump(), "application/json");
             return;
        }
        
        game->resetGame();
        res.set_content("{\"status\":\"success\"}", "application/json");
    });

    // Preflight handler
    svr.Options(R"(/api/.*)", [](const Request&, Response& res) {
        add_cors_headers(res);
        res.status = 204;
    });

    std::cout << "Ludo Server starting at http://localhost:8080" << std::endl;
    svr.listen("0.0.0.0", 8080);
    
    return 0;
}
