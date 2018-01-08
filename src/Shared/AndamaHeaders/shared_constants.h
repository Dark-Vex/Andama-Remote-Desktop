#ifndef SHARED_CONSTANTS_H
#define SHARED_CONSTANTS_H

const int PROXY_PORT_NUMBER = 80; //PROXY listening port
const std::string PROXY_HOST_NAME = "mailgate.filoxeni.com";
//const std::string PROXY_HOST_NAME = "andamaproxy-us-west.filoxeni.com";
//const std::string PROXY_HOST_NAME = "andamaproxy-ro-timisoara.filoxeni.com";
//const std::string PROXY_HOST_NAME = "localhost";

const std::array<char,1> CMD_PROTOCOL = {{'P'}};
const std::array<char,1> CMD_CONNECT = {{'C'}};
const std::array<char,1> CMD_CONNECT_ID_NOT_FOUND = {{'c'}}; // stelnetai apo ton server otan den vrethei to id pou zitithike apo CMD_CONNECT
const std::array<char,1> CMD_QUIT = {{'Q'}};
const std::array<char,1> CMD_ACCEPT = {{'A'}};
const std::array<char,1> CMD_ID = {{'I'}};
const std::array<char,1> CMD_SCREENSHOT = {{'S'}};
const std::array<char,1> CMD_SCREENSHOT_DIFF = {{'s'}};
const std::array<char,1> CMD_REQUEST_SCREENSHOT = {{'R'}};
const std::array<char,1> CMD_REQUEST_SCREENSHOT_DIFF = {{'r'}};
const std::array<char,1> CMD_MOUSE = {{'M'}}; // mouse
const std::array<char,1> CMD_MOUSE_CURSOR = {{'m'}}; // mouse cusror type
const std::array<char,1> CMD_KEYBOARD = {{'K'}}; // KEYBOARD
const std::array<char,1> CMD_DISCONNECT_FROM_REMOTE_COMPUTER = {{'D'}}; // disconnect from remote computer
const std::array<char,1> CMD_HEART_BEAT = {{'H'}};
const std::array<char,1> CMD_BAN_IP_WRONG_ID = {{'B'}}; // synolika 5 bytes
const std::array<char,1> CMD_WARNING_BAN_IP_WRONG_ID= {{'W'}}; // synolika 2 bytes
const std::array<char,1> CMD_CONNECT_PASSWORD_NOT_CORRECT= {{'p'}}; // synolika 1 byte
const std::array<char,1> CMD_BAN_IP_WRONG_PWD = {{'b'}}; //synolika ena byte (mono to commanfd)
const std::array<char,1> CMD_WARNING_BAN_IP_WRONG_PWD= {{'w'}}; // synolika 2 bytes
const std::array<char,1> CMD_ERROR_APP_VERSION_NOT_ACCEPTED = {{'V'}}; // epistrefei to URL pros ti selida pou tha katevasei o xristis tin neoteri ekdosi tis efarmogis
const std::array<char,1> CMD_ERROR_PROTOCOL_VERSION_NOT_ACCEPTED = {{'v'}}; // (mono command)

const std::array<char,1> CMD_P2P_PROTOCOL_OK = {{'d'}}; //o p2pserver stelnei afto to command enos byte otan to client protocol einai ok
const std::array<char,1> CMD_P2P_CONNECT = {{'N'}}; //o p2pserver stelnei afto to command enos byte otan to client protocol einai ok
const std::array<char,1> CMD_P2P_CLIENT_UPNP_PORT = {{'U'}}; //o client stelnei ston server tin UPnP port poy exei anoiksei wste na dexetai P2P connections
const std::array<char,1> CMD_P2P_REMOTE_CLIENT_UPNP_PORT = {{'u'}}; // Ean o remote client poy zitithike syndexei exei UPnP port anoixti, o proxy stelnei ston client to ID kai UPnP port tou remote client wste na ginei prospatheia P2P syndesis
#endif // SHARED_CONSTANTS_H
