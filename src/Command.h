#ifndef __CLIENT_CMD_H_H__
#define __CLIENT_CMD_H_H__

/** Command is type of 'unsigned short'. **/
enum Command
{
        /* client to server */
        C2S_Login = 1024,
        C2S_Chat = 1025,

        /* server to client */
        S2C_Chat = 2048,

        /* server to server */
        S2S_Chat = 3072,

        /* max command */
        COMMAND_MAX = 65535,
};

#endif