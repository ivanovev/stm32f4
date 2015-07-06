
#include "eth/eth.h"
#include "tftpd.h"

#ifdef ENABLE_FLASH
#include "flash/flash.h"
#endif

extern volatile uint8_t reset;

enum
{
    TFTP_RRQ    = 1,
    TFTP_WRQ    = 2,
    TFTP_DATA   = 3,
    TFTP_ACK    = 4,
    TFTP_ERR    = 5,
    TFTP_OACK   = 6,
    SEG_SZ      = 512,
};

enum
{
    TFTP_EUNDEF = 0,
    TFTP_ENOTFOUND,
    TFTP_EACCESS,
    TFTP_ENOSPACE,
    TFTP_EBADOP,
    TFTP_EBADID,
    TFTP_EEXISTS,
    TFTP_ENOUSER,
};

static struct errmsg {
    int16_t      e_code;
    const char  *e_msg;
} errmsgs[] = {
    {TFTP_EUNDEF,   "Undefined error"},
    {TFTP_ENOTFOUND,    "File not found"},
    {TFTP_EACCESS,  "Access violation"},
    {TFTP_ENOSPACE, "Disk full or allocation exceeded"},
    {TFTP_EBADOP,   "Illegal TFTP operation"},
    {TFTP_EBADID,   "Unknown transfer ID"},
    {TFTP_EEXISTS,  "File already exists"},
    {TFTP_ENOUSER,  "No such user"},
    {-1,            0}
};

struct {
    uint16_t port;
    uint16_t ackn;
    uint16_t mode;
    uint32_t crc;
    uint32_t start, end;
} tfs;

void myip_tftpd_init(void)
{
    tfs.port = 55885;
    tfs.ackn = 0;
    tfs.crc = 0;
    tfs.mode = 0;
}

static uint16_t tftpd_nak(uint8_t *out, int16_t e_code)
{
    TFTP_ERR_PKT *err = (TFTP_ERR_PKT*)out;
    struct errmsg *e = 0;
    err->opcode = HTONS_16(TFTP_ERR);
    err->e_code = HTONS_16(e_code);
    uint16_t i, sz = 4;
    for(i = 0;; i++)
    {
        e = &errmsgs[i];
        if(e->e_code == e_code)
        {
            sz += mysnprintf(err->e_msg, SEG_SZ, "%s", e->e_msg);
            sz += 1;
            break;
        }
        if(e->e_code == -1)
            break;
    }
    out[sz] = 0;
    return sz;
}

static uint16_t tftpd_ack(uint8_t *out)
{
    TFTP_ACK_PKT *ack = (TFTP_ACK_PKT*)out;
    ack->opcode = HTONS_16(TFTP_ACK);
    ack->ackn = HTONS_16(tfs.ackn);
    tfs.ackn++;
    return 4;
}

uint16_t tftpd_data_in(uint8_t *in, uint16_t sz, uint8_t *out)
{
#ifdef ENABLE_FLASH
    TFTP_DATA_PKT *pkt = (TFTP_DATA_PKT*)in;
    sz -= 4;
    if(sz)
    {
        flash_write_array(tfs.start, pkt->data, sz);
        tfs.start += sz;
    }
    if(sz != SEG_SZ)
    {
        uint32_t fsz = flash_fsz1();
        uint32_t crc = flash_crc1(fsz);
        dbg_send_hex2("crc_orig", tfs.crc);
        dbg_send_hex2("crc_recv", crc);
        if(crc && (crc == tfs.crc))
        {
            reset = RESET_FWUPG;
        }
    }
#endif
    return tftpd_ack(out);
}

uint16_t tftpd_data_out(uint8_t *out)
{
    TFTP_DATA_PKT *pkt = (TFTP_DATA_PKT*)out;
    pkt->opcode = HTONS_16(TFTP_DATA);
    pkt->ackn = HTONS_16(tfs.ackn);
    tfs.ackn++;
    uint16_t chunk_sz = SEG_SZ;
    if(chunk_sz >= (tfs.end - tfs.start))
    {
        chunk_sz = (tfs.end - tfs.start);
    }
    mymemcpy(pkt->data, (uint8_t*)tfs.start, chunk_sz);
    tfs.start += chunk_sz;
    return chunk_sz + 4;
}

uint16_t tftpd_wrq(uint8_t *in, uint16_t sz, uint8_t *out)
{
    TFTP_PKT *pkt = (TFTP_PKT*)in;
    char *name = (char*)pkt->data;
    uint16_t len = mystrnlen(name, SEG_SZ);
    uint16_t i;
    char *mode = name + len + 1;
    dbg_send_str3(name, 1);
    dbg_send_str3(mode, 1);
    tfs.ackn = 0;
    if(mystrncmp(mode, "binary", SEG_SZ) && mystrncmp(mode, "octet", SEG_SZ))
    {
        return tftpd_nak(out, TFTP_EBADOP);
    }
    if((name[8] == '.') && !mystrncmp(&name[9], "bin", 3))
    {
        for(i = 0; i < 8; i++)
        {
            if(!myisxdigit(name[i]))
                return tftpd_nak(out, TFTP_EBADOP);
        }
        tfs.crc = htoi(name);
    }
    else if(!mystrncmp(name, "script.pcl", 10))
    {
        tfs.crc = 0;
    }
    else
        return tftpd_nak(out, TFTP_ENOTFOUND);
    tfs.mode = TFTP_WRQ;
#ifdef ENABLE_FLASH
    tfs.start = USER_FLASH_MID_ADDR;
    i = flash_erase1();
    dbg_send_int2("flash_erase1", i);
    HAL_FLASH_Unlock();
#endif
    return tftpd_ack(out);
}

uint16_t tftpd_rrq(uint8_t *in, uint16_t sz, uint8_t *out)
{
    TFTP_PKT *pkt = (TFTP_PKT*)in;
    char *name = (char*)pkt->data;
    uint16_t len = mystrnlen(name, SEG_SZ);
    char *mode = name + len + 1;
    dbg_send_str3(name, 1);
    dbg_send_str3(mode, 1);
    tfs.ackn = 1;
#ifdef ENABLE_FLASH
    if(mystrncmp(mode, "binary", SEG_SZ) && mystrncmp(mode, "octet", SEG_SZ))
    {
        return tftpd_nak(out, TFTP_EBADOP);
    }
    if((name[6] == '.') && !mystrncmp(&name[7], "bin", 3))
    {
        if(name[5] == '0')
        {
            tfs.start = USER_FLASH_START_ADDR;
            tfs.end = tfs.start + flash_fsz0();
        }
        else if(name[5] == '1')
        {
            tfs.start = USER_FLASH_MID_ADDR;
            tfs.end = tfs.start + flash_fsz1();
        }
        else
            return tftpd_nak(out, TFTP_ENOTFOUND);
        tfs.mode = TFTP_RRQ;
        return tftpd_data_out(out);
    }
    else if(!mystrncmp(name, "script.pcl", 10))
    {
        tfs.start = USER_FLASH_MID_ADDR;
        tfs.end = tfs.start + flash_fsz1();
        tfs.mode = TFTP_RRQ;
        return tftpd_data_out(out);
    }
#endif
    return tftpd_nak(out, TFTP_ENOTFOUND);
}

uint16_t myip_tftpd_con_handler(uint8_t *in, uint16_t sz, uint8_t *out)
{
    if(sz == 0)
        return 0;
    TFTP_PKT *pkt = (TFTP_PKT*)in;
    if(HTONS_16(pkt->opcode) == TFTP_WRQ)
        return tftpd_wrq(in, sz, out);
    if(HTONS_16(pkt->opcode) == TFTP_RRQ)
        return tftpd_rrq(in, sz, out);
    else if(HTONS_16(pkt->opcode) == TFTP_DATA)
        return tftpd_data_in(in, sz, out);
    else if(HTONS_16(pkt->opcode) == TFTP_ACK)
        return tftpd_data_out(out);
    return 0;
}

