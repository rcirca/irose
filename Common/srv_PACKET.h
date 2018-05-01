#ifndef __GL_PACKET_INC
#define __GL_PACKET_INC
//-------------------------------------------------------------------------------------------------

// login server <--> game server
#define GSV_ACTIVE              0x07f01
#define GSV_SHUTDOWN            0x07f02
#define GSV_ADD_CLIENT          0x07f03
#define GSV_SUB_CLIENT          0x07f04

struct gsv_ACTIVE {
    t_PACKETHEADER  m_H;
    short           m_nPort;
    // szServerName[];
} ;

struct gsv_ADD_ZONE {
    t_PACKETHEADER  m_H;
    short           m_nPort;
    // szZoneName[];
} ;
struct gsv_SHUTDOWN {
    t_PACKETHEADER  m_H;
} ;
struct gsv_ADD_CLIENT {
    t_PACKETHEADER  m_H;
    // szAccount[];
} ;
struct gsv_SUB_CLIENT {
    t_PACKETHEADER  m_H;
    // szAccount[];
} ;

//-------------------------------------------------------------------------------------------------
#endif

 