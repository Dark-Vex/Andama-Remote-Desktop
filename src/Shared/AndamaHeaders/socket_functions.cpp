#include "socket_functions.h"

#ifdef WIN32
#define NOMINMAX
#include <stdio.h>
#include "winsock2.h"
#include <ws2tcpip.h>
#define bzero(b,len) (memset((b), '\0', (len)), (void) 0)
#define bcopy(b1,b2,len) (memmove((b2), (b1), (len)), (void) 0)
#define in_addr_t uint32_t
#pragma comment(lib, "Ws2_32.lib")

#else
#include <unistd.h>
#include <arpa/inet.h>
#endif

//#include "../Shared/General/bench.h"

std::mutex send_mutex; //sygxronismos sockets.TODO (xreiazetai sygxronismos wste se kamia periptwsi na mi ginetai apo diaforetika thread lipsi i apostoli sto idio socket

//apostelei to command(1byte) kai to mynima efoson yparxei
#ifdef WIN32
int _sendmsgPlain(const SOCKET socketfd, const std::vector<char> &message)
#else
int _sendmsgPlain(const int socketfd, const std::vector<char> &message)
#endif
{
    std::size_t total = 0;        // how many bytes we've sent
    { // lock_guard scope
        std::lock_guard<std::mutex> lock(send_mutex);

        //prepei na vevaiwthooume oti stalthike olo to mynhma
        size_t bytesleft = message.size(); // how many we have left to send
        int n;
        while (total < message.size()){
            n = send(socketfd,message.data()+total,bytesleft,0);
            if (n < 0){
                break;
            }
            else if (n == 0)
            {
                std::cout << "-----> send returned 0 bytes. Expected: " << message.size() <<
                             "  [ int clientserver::_sendmsgPlain(int socketfd ,const std::vector<char> &message) ]" << std::endl;
                displayErrno("int clientserver::_sendmsgPlain(int socketfd, const std::vector<char> &message)");
                return 0;
            }
            total+=n;
            bytesleft-=n;
        }
    } // lock_guard scope

    if (total != message.size()){
        std::cout << "----> throw EXCEPTION IN int clientserver::_sendmsgPlain(int socketfd, const std::vector<char> &message). Bytes send not equal to bytes expected." << std::endl;
        displayErrno("int clientserver::_sendmsgPlain(int socketfd, const std::vector<char> &message)");
        throw std::runtime_error(std::string("----> throw EXCEPTION IN int clientserver::_sendmsgPlain(int socketfd, const std::vector<char> &message). Bytes send not equal to bytes expected."));
    }
    return total;
}

//apostelei to command(1byte) kai to mynima efoson yparxei
#ifdef WIN32
int _sendmsgPlain(const SOCKET socketfd, const std::array<char, 1> &command, const std::vector<char> &message)
#else
int _sendmsgPlain(const int socketfd, const std::array<char, 1> &command, const std::vector<char> &message)
#endif
{
    std::vector<char> msg(1);
    msg[0]=command[0];

    if (message.size() > 0){
        msg.insert(msg.begin()+1, message.begin(),message.end());
    }

    return _sendmsgPlain(socketfd,msg);
}


/* apostelei:
 * - to command (1 byte)
 * - ypologizei me 4 byte to megethos tou mynimatos
 * - to mynima
 * - Veltiwseis pou tha mporousan na ginoun:
 * a)anti na antigrafw to message sto msg, na kataxwrw to msg stin arxi tou message
 */
#ifdef WIN32
int _sendmsg(const SOCKET socketfd, const std::array<char, 1> &command, const std::vector<char> &message)
#else
int _sendmsg(const int socketfd,    const std::array<char, 1> &command, const std::vector<char> &message)
#endif
{
    //std::cout << "clientserver::sendmsg called" << std::endl;

    std::vector<char> msg(5);
    int len = message.size();

    //std::cout << "16. Message len: " << len << std::endl;

    std::vector<char> lenb(4);
    intToBytes(len,lenb);

    msg[0]=command[0];
    msg[1]=lenb[0];
    msg[2]=lenb[1];
    msg[3]=lenb[2];
    msg[4]=lenb[3];

    msg.insert(msg.begin()+5, message.begin(),message.end());

    std::size_t total = 0;        // how many bytes we've sent
    { // lock_guard scope
        //std::cout << "17. Will lock send socket (sendmutex)" << std::endl;
        std::lock_guard<std::mutex> lock (send_mutex);
        //send_mutex.lock();
        //std::cout << "18. Will send Command: " << &msg.at(0) <<  ". Total bytes: " << msg.size() << std::endl;

        //prepei na vevaiwthooume oti stalthike olo to mynhma
        size_t bytesleft = msg.size(); // how many we have left to send
        int n;
        while (total < msg.size()){
            //Bench bench("_sendmsg");
            n = send(socketfd,msg.data()+total,bytesleft,0);
            if (n < 0){
                break;
            }
            else if (n == 0)
            {
                std::cout << "-----> send returned 0 bytes. Expected: " << msg.size() <<
                             "  [ int clientserver::_sendmsg(int socketfd,    const std::array<char, 1> &command, const std::vector<char> &message) ]" << std::endl;
                displayErrno("int clientserver::_sendmsg(int socketfd,    const std::array<char, 1> &command, const std::vector<char> &message)");
                return 0;
            }
            total+=n;
            bytesleft-=n;
        }

        //std::cout << "19. Command: " << &msg[0] << " send. Total bytes returned from socket: " << total << std::endl;
    } // lock_guard scope
    //std::cout << "############ 19. send socket unlocked (sendmutex). END" << std::endl;

    if (total != msg.size()){
        std::cout << "----> throw EXCEPTION IN size_t int clientserver::_sendmsg(int socketfd,    const std::array<char, 1> &command, const std::vector<char> &message). Bytes send not equal to bytes expected. Command: %s" << command[0] << std::endl;
        throw std::runtime_error(std::string("----> throw EXCEPTION IN size_t int clientserver::_sendmsg(int socketfd,    const std::array<char, 1> &command, const std::vector<char> &message). Bytes send not equal to bytes expected."));
    }
    return total;
}

//lamvanei bytes isa me to length tou charbuffer pou pernaei byref
#ifdef WIN32
int _receivePlain(const SOCKET socketfd, std::vector<char> &charbuffer)
#else
int _receivePlain(const int socketfd, std::vector<char> &charbuffer)
#endif
{
    size_t bytes_cnt_payload=0;

    while(bytes_cnt_payload < charbuffer.size())
    {
        int bytes_rcv_payload = recv(socketfd,charbuffer.data() + bytes_cnt_payload,charbuffer.size() - bytes_cnt_payload, 0);

        if (bytes_rcv_payload == 0){
            std::cout << std::this_thread::get_id() << " " <<
                       "####  int recieve: recv return 0 bytes. [int _receivePlain(int socketfd, std::vector<char> &charbuffer)]. Returning from function." << std::endl;
            return 0;
        }
        else if (bytes_rcv_payload == -1){
            displayErrno("int _receivePlain(int socketfd, std::vector<char> &charbuffer)");
            return -1;
        }

        bytes_cnt_payload += bytes_rcv_payload;
    }

    if (bytes_cnt_payload != charbuffer.size()){
        std::cout << "----> EXCEPTION IN recieve func int _receivePlain(int socketfd, std::vector<char> &charbuffer). Bytes received not equal to bytes expected. Expected: " << charbuffer.size() << " received: " << bytes_cnt_payload << std::endl;
        displayErrno("int clientserver::_sendmsg(int socketfd,    const std::array<char, 1> &command, const std::vector<char> &message)");
        throw std::runtime_error(std::string("----> EXCEPTION IN recieve func (payload). Bytes received not equal to bytes expected.\n"));
    }

    return bytes_cnt_payload;
}

/* kanei de-serialize ta prwta 4 bytes tou minimatos
 * pou perigrafoun to megethos tou mynimatos pou akolouthei
 * kai sti synexei lamvanoun to mynima.
 * Afto to mynima to fortwnei sto charbuffer byref
 */
#ifdef WIN32
int _receive(const SOCKET socketfd, std::vector<char> &charbuffer)
#else
int _receive(const int socketfd, std::vector<char> &charbuffer)
#endif
{
    //-----------
    //prwto meros: lamvano to synolo twn bytes poy anamenontai
    //sto payload
    size_t bytes_needed = 4;
    std::vector<char> len_bytes(4);
    size_t bytes_cnt=0;

    while(bytes_cnt < bytes_needed)
    {
        int bytes_rcv = recv(socketfd, len_bytes.data() + bytes_cnt, bytes_needed - bytes_cnt, 0);

        if (bytes_rcv == 0){
            std::cout << std::this_thread::get_id() << " " <<
                       "####  int recieve: recv return 0 bytes. [first while loop]. Returning from function." << std::endl;
            return 0;
        }
        else if (bytes_rcv == -1){
            displayErrno("int _receive(int socketfd, std::vector<char> &charbuffer) - first while loop");
            return -1;
        }

        bytes_cnt += bytes_rcv;
    }

    if (bytes_needed != bytes_cnt){
        std::cout << "----> EXCEPTION IN recieve func (payload length). Bytes received not equal to bytes expected. Expected: " << bytes_needed << " received: " << bytes_cnt << std::endl;
        displayErrno("int _receive(int socketfd, std::vector<char> &charbuffer) - first while loop");
        throw std::runtime_error(std::string("----> EXCEPTION IN recieve func (payload length). Bytes received not equal to bytes expected."));
    }

    //-----------

    //deftero meros: lamvanw to payload
    //std::cout << "_recieve will call bytesToInt to compute required msg size" << std::endl;
    size_t bytes_needed_payload = bytesToInt(len_bytes);
    //std::cout << "_recieve required msg size: " << bytes_needed_payload << " >> charbuffer.max_size():" << charbuffer.max_size() << " Will resize charbuffer..." << std::endl;
    //SIMANTIKO!!! An syndethw me telnet kai kataxwrisw P010 o server krasarei se linux giati
    //apo oti katalavainw den mporei na kanei resize to charbuffer se ena poly megalo arithmo.
    //Gi afto kanw ton elegxo parakatw pou tha prostatevei kai ton server se periptwsi pou kapoios paei na
    //steilei p.x. panw apo 20mb dedomenwn gia na tou dimiourgisei provlima.
    if (bytes_needed_payload < 20971520){ //maximum peripou 20 MB gia prostasia
        charbuffer.resize(bytes_needed_payload);
    }
    else {
        std::cout << "Error on _receive: cannot receive more than 20971520 bytes at once" << std::endl;
        throw std::runtime_error("_receive > cannot receive more than 20971520 bytes at once");
    }
    size_t bytes_cnt_payload = 0;

    while(bytes_cnt_payload < bytes_needed_payload)
    {
        int bytes_rcv_payload = recv(socketfd,charbuffer.data() + bytes_cnt_payload,bytes_needed_payload - bytes_cnt_payload, 0);

        if (bytes_rcv_payload == 0){
            std::cout << std::this_thread::get_id() << " " <<
                       "####  int recieve: recv return 0 bytes. [second while loop]. Returning from function." << std::endl;
            return 0;
        }
        else if (bytes_rcv_payload == -1){
            displayErrno("int _receive(int socketfd, std::vector<char> &charbuffer) - second while loop");
            return -1;
        }

        bytes_cnt_payload += bytes_rcv_payload;
    }

    if (bytes_cnt_payload != bytes_needed_payload){
        std::cout << "----> EXCEPTION IN recieve func (payload). Bytes received not equal to bytes expected. Expected: " << bytes_needed_payload << " received: " << bytes_cnt_payload << std::endl;
        displayErrno("int _receive(int socketfd, std::vector<char> &charbuffer) - second while loop");
        throw std::runtime_error(std::string("----> EXCEPTION IN recieve func (payload). Bytes received not equal to bytes expected."));
    }

    return bytes_cnt_payload + bytes_cnt;
}

int getClientSocket(std::string IP, unsigned short int Port)
{
        struct sockaddr_in serv_addr;

        #ifdef WIN32
            // Initialize Winsock
            int iResult;
            WSADATA wsaData;
            iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
            if (iResult != 0) {
                std::cout << "WSAStartup failed: " << iResult << std::endl;
                return -1;
            }
        #endif

        int sock = socket(AF_INET, SOCK_STREAM, 0);
#ifdef WIN32
        if (sock == INVALID_SOCKET) {
#else
        if (sock < 0){
#endif
            perror("ERROR opening socket");
            return -1;
        }


        memset((char *) &serv_addr,0, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr=inet_addr(IP.data());
        serv_addr.sin_port = htons(Port);

        int conres = ::connect(sock,(struct sockaddr *) &serv_addr,sizeof(serv_addr));
        if (conres < 0)
        {
            std::cout << "ERROR connecting. result: " << conres << "\n";

#ifdef WIN32
            closesocket(sock);
#else
            close(sock);
#endif
            //edw xtypaei ean yparxei syndesi sto internet alla o proxy den trexei
            //error("ERROR connecting");

            return -1;
         }

        return sock;
}

//anti gia tin inet_pton pou den yparxei sto mingw compiler
//https://stackoverflow.com/questions/15370033/how-to-use-inet-pton-with-the-mingw-compiler
int inet_pton_mingw(int af, const char *src, char *dst)
{
    switch (af)
    {
    case AF_INET:
        return inet_pton4_mingw(src, dst);
    case AF_INET6:
        return inet_pton6_mingw(src, dst);
    default:
        return -1;
    }
}
int inet_pton4_mingw(const char *src, char *dst)
{
    uint8_t tmp[NS_INADDRSZ], *tp;

    int saw_digit = 0;
    int octets = 0;
    *(tp = tmp) = 0;

    int ch;
    while ((ch = *src++) != '\0')
    {
        if (ch >= '0' && ch <= '9')
        {
            uint32_t n = *tp * 10 + (ch - '0');

            if (saw_digit && *tp == 0)
                return 0;

            if (n > 255)
                return 0;

            *tp = n;
            if (!saw_digit)
            {
                if (++octets > 4)
                    return 0;
                saw_digit = 1;
            }
        }
        else if (ch == '.' && saw_digit)
        {
            if (octets == 4)
                return 0;
            *++tp = 0;
            saw_digit = 0;
        }
        else
            return 0;
    }
    if (octets < 4)
        return 0;

    memcpy(dst, tmp, NS_INADDRSZ);

    return 1;
}
int inet_pton6_mingw(const char *src, char *dst)
{
    static const char xdigits[] = "0123456789abcdef";
    uint8_t tmp[NS_IN6ADDRSZ];

    uint8_t *tp = (uint8_t*) memset(tmp, '\0', NS_IN6ADDRSZ);
    uint8_t *endp = tp + NS_IN6ADDRSZ;
    uint8_t *colonp = NULL;

    /* Leading :: requires some special handling. */
    if (*src == ':')
    {
        if (*++src != ':')
            return 0;
    }

    const char *curtok = src;
    int saw_xdigit = 0;
    uint32_t val = 0;
    int ch;
    while ((ch = tolower(*src++)) != '\0')
    {
        const char *pch = strchr(xdigits, ch);
        if (pch != NULL)
        {
            val <<= 4;
            val |= (pch - xdigits);
            if (val > 0xffff)
                return 0;
            saw_xdigit = 1;
            continue;
        }
        if (ch == ':')
        {
            curtok = src;
            if (!saw_xdigit)
            {
                if (colonp)
                    return 0;
                colonp = tp;
                continue;
            }
            else if (*src == '\0')
            {
                return 0;
            }
            if (tp + NS_INT16SZ > endp)
                return 0;
            *tp++ = (uint8_t) (val >> 8) & 0xff;
            *tp++ = (uint8_t) val & 0xff;
            saw_xdigit = 0;
            val = 0;
            continue;
        }
        if (ch == '.' && ((tp + NS_INADDRSZ) <= endp) &&
                inet_pton4_mingw(curtok, (char*) tp) > 0)
        {
            tp += NS_INADDRSZ;
            saw_xdigit = 0;
            break; /* '\0' was seen by inet_pton4(). */
        }
        return 0;
    }
    if (saw_xdigit)
    {
        if (tp + NS_INT16SZ > endp)
            return 0;
        *tp++ = (uint8_t) (val >> 8) & 0xff;
        *tp++ = (uint8_t) val & 0xff;
    }
    if (colonp != NULL)
    {
        /*
         * Since some memmove()'s erroneously fail to handle
         * overlapping regions, we'll do the shift by hand.
         */
        const int n = tp - colonp;

        if (tp == endp)
            return 0;

        for (int i = 1; i <= n; i++)
        {
            endp[-i] = colonp[n - i];
            colonp[n - i] = 0;
        }
        tp = endp;
    }
    if (tp != endp)
        return 0;

    memcpy(dst, tmp, NS_IN6ADDRSZ);

    return 1;
}



