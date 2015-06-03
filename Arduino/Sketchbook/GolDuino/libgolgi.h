//
// Golgi-WiFi v2-08-05
//

#ifndef __LIBGOLGI_H__
#define __LIBGOLGI_H__
#include <Arduino.h>
#include <stddef.h>
#include <stdlib.h>

#ifdef __ARDUINO_X86__
//
// Probably Edison
//
#define __EDISON__ 1
#endif

# 1 ".libgolgi.h"
# 1 "<built-in>"
# 1 "<command-line>"
# 1 ".libgolgi.h"
# 1 "/slingshot/golgi/garrick/v2/22/02/cpp/GolDuinoExc.h" 1
# 13 "/slingshot/golgi/garrick/v2/22/02/cpp/GolDuinoExc.h"
class GolgiException{
public:
    GolgiException(void);
    GolgiException(bool isSetDefault);
    GolgiException *dupe(void);
    virtual ~GolgiException(void);

    bool isCorrupt(void);

    bool errTextIsSet(void);
    const char *getErrText(void);
    void setErrText(const char *val);

    bool errTypeIsSet(void);
    int32_t getErrType(void);
    void setErrType(int32_t val);

    bool decodeFromPayload(const char *payload);
    const char *encode(void);
    const char *encodeWithPrefix(const char *prefix);

private:
    void initialise(bool isSetDefault);
    bool _corrupt;

    const char *_errText;
    bool _errTextIsSet;

    int32_t _errType;
    bool _errTypeIsSet;
};
# 2 ".libgolgi.h" 2
# 1 "GolgiAPIImpl.h" 1
# 11 "GolgiAPIImpl.h"
# 1 "GolgiMessage.h" 1
# 11 "GolgiMessage.h"
# 1 "NTLPayload.h" 1
# 11 "NTLPayload.h"
# 1 "TBX.h" 1
# 11 "TBX.h"
struct TbxString{
    char *buf;
    int32_t bufLen;
    int32_t strLen;
};
# 31 "TBX.h"
extern const char *tbxToHex(unsigned long val);
extern char *tbxFromHex (unsigned long *dst, const char *hex);
extern char *tbxEatWhite(const char *s);
extern char *tbxEatBlack(const char *s);
extern char *tbxSubStrdup(const char *bos, const char *eos);
extern int32_t tbxGet3DigitCode(const char *s);
extern char *tbxEscapeString(const char *src);
extern char *tbxDeEscapeString(const char *src);
extern void tbxFree(void *addr);

extern struct TbxString *tbxStrcat(struct TbxString *str, const char *str2add);
extern struct TbxString *tbxStrcatData(struct TbxString *dst, const unsigned char *data, int32_t len);
extern struct TbxString *tbxStrcpy(struct TbxString *str, const char *str2cpy);
extern void tbxStringRelease(struct TbxString *str);
extern const char *tbxItos(int32_t val);
extern const char *tbxDtos(double dval);
extern double tbxStod(const char *str, int32_t *err);
# 12 "NTLPayload.h" 2

struct TvPair{
    struct TvPair *blink, *flink;
    const char *tag;
    const char *value;
};

class NTLPayload {
public:
        NTLPayload();
        const char *charData();
        void addChar(char ch);
        void addString(const char *string);
        const char *readLine(void);
        bool isComplete();
        int32_t lineCount();
        int32_t parse();
        const char *getString(const char *tag, int32_t *notFound);
        unsigned char *getData(const char *tag, int32_t *notFound, int32_t *lenPtr);
        int32_t getInt(const char *tag, int32_t *notFound);
        double getDouble(const char *tag, int32_t *notFound);
        const char *getNestedPayload(const char *prefix);
        virtual ~NTLPayload();

private:
        char *buf;
        int32_t bufSize;
        int32_t bufWr;
        int32_t bufRd;
        int32_t llen;
        int32_t lines;
        bool wasEol;
        bool dotEscaped;
        bool complete;
        struct TvPair *tvRoot;

        struct TvPair *locateTag(const char *tag);
};
# 12 "GolgiMessage.h" 2


struct GmOption{
    struct GmOption *blink, *flink;
    const char *key;
    const char *val;
};

class GolgiMessage {
public:
    GolgiMessage();
    virtual ~GolgiMessage();
    GolgiMessage *dupe();

    int32_t decode(NTLPayload *src);
    void encode(NTLPayload *dst, const char *payload);
    void encode(NTLPayload *dst);
    const char *dev_key;
    const char *api_key;
    const char *oa_app_user_id;
    const char *da_app_user_id;
    const char *message_id;
    int32_t msg_type;
    const char *method;
    int32_t err_type;
    const char *err_txt;
    const char *payload;
    struct GmOption *optionRoot;
};
# 12 "GolgiAPIImpl.h" 2
# 1 "GolgiSendMsgImpl.h" 1
# 11 "GolgiSendMsgImpl.h"
# 1 "NTLConn.h" 1
# 14 "NTLConn.h"
class GolgiNetInterface{
public:
    virtual bool connect(const char *host, int32_t port) {};
    virtual bool connected(void) {};
    virtual void service(void) {};
    virtual void stop(void) {};
    virtual bool needsNops() { return true;};
    virtual bool packetized() {return false;};





    virtual void write(const char *str) {};
    virtual int32_t available(void) {};
    virtual int32_t read(void) {};
    virtual int32_t read(void *dst, int32_t len) {};





    virtual void sendReq(unsigned long tid, const char *data) {};
    virtual void sendRes(unsigned long tid, const char *data) {};
};


class NTLConn;
class NTLResponseHandler{
public:
    virtual void ntlResponse(int32_t rc, const char *rcStr, NTLPayload *payload) {};
};

class NTLRequestHandler{
public:
    virtual void ntlRequest(NTLConn *conn, unsigned long tid, NTLPayload *payload) {};
};

class NopSender;


struct NtlResHandlerEntry{
    struct NtlResHandlerEntry *flink, *blink;
    NTLResponseHandler *responseHandler;
    unsigned long tid;
};

struct NtlReqHandlerEntry{
    struct NtlReqHandlerEntry *blink, *flink;
    const char *cmd;
    NTLRequestHandler *requestHandler;
};

class NTLConn {
public:
    NTLConn(GolgiNetInterface *netIf);
    void setCredentials(const char *appKey, const char *devKey, const char *instId);
    bool connect(const char *host, int32_t port);
    bool connected();
    void addCmdHandler(const char *cmd, NTLRequestHandler *reqHandler);
    void sendCmd(const char *cmd, NTLResponseHandler *rspHandler);
    void sendCmd(const char *cmd, NTLPayload *payload, NTLResponseHandler *rspHandler);
    void sendRsp(unsigned long tid, int32_t rc, const char *rcStr, NTLPayload *payload);
    void sendRsp(unsigned long tid, int32_t rc, const char *rcStr);
    void sendRspOTW(unsigned long tid, const char *data);
    void sendReqOTW(unsigned long tid, const char *data);
    void service();
    void setDestination(const char *host, int32_t port);
    virtual ~NTLConn();
private:
    GolgiNetInterface *netIf;
    struct NtlResHandlerEntry *resHandlerList;
    struct NtlReqHandlerEntry *reqHandlerList;
    void processLine();
    void reconnect();
    const char *appKey;
    const char *devKey;
    const char *instId;
    const char *host;
    int32_t port;
    char *tid;
    char *lbuf;
    int32_t lbufWr;
    int32_t lbufSz;
    bool gathering;
    bool golgiRegistered;
    bool bannerSeen;
    bool disconnectSeen;
    unsigned long discoTime;
    unsigned long recoTime;
    unsigned long nextTid;
    int32_t recoCount;
    NopSender *nopSender;
    NTLPayload *payload;
    char *currentLine;
    bool haveCredentials;
};
# 12 "GolgiSendMsgImpl.h" 2

# 1 "GolgiAPIImpl.h" 1
# 14 "GolgiSendMsgImpl.h" 2


class GolgiAPIImpl;
class GolgiResultReceiver;

class GolgiSendMsgImpl:public NTLRequestHandler {
public:
    GolgiSendMsgImpl(GolgiAPIImpl *apiImpl);
    virtual ~GolgiSendMsgImpl();

    void ntlRequest(NTLConn *conn, unsigned long tid, NTLPayload *payload);

private:
    GolgiAPIImpl *apiImpl;
};
# 13 "GolgiAPIImpl.h" 2

extern void setCryptoImpl(const char *(*enc)(const char *dst,
                                             const char *payload,
                                             const char **err),
                          const char *(*dec)(const char *src,
                                             const char *payload,
                                             const char **err));

extern int32_t _useTestGolgiServers;
class GolgiAPIImpl;

class GolgiResultSender:public NTLResponseHandler{
public:
    GolgiResultSender(GolgiAPIImpl *apiImpl, GolgiMessage *golgiMessage);
    virtual ~GolgiResultSender();
    void sendResult(const char *payload);
    void sendError(int32_t err_type, const char *err_txt);
    void ntlResponse(int32_t rc, const char *rcStr, NTLPayload *payload);
    const char *getSender(void);
private:
    GolgiAPIImpl *apiImpl;
    GolgiMessage *golgiMessage;
    const char *senderId;
};

class GolgiRequestReceiver{
public:
    virtual void inboundRequest(GolgiResultSender *resultSender, const char *payload) = 0;
    virtual ~GolgiRequestReceiver() {};
};

class GolgiResultReceiver{
public:
    virtual void inboundResult(const char *payload) = 0;
    virtual void inboundError(int32_t errType, const char *errText) = 0;
    virtual ~GolgiResultReceiver() {};
};



struct GolgiReqHandlerEntry{
    struct GolgiReqHandlerEntry *blink, *flink;
    const char *method;
    NTLRequestHandler *requestHandler;
};



struct GolgiRequestReceiverEntry{
    struct GolgiRequestReceiverEntry *blink, *flink;
    const char *method;
    GolgiRequestReceiver *receiver;
};

struct GolgiResultReceiverEntry{
    struct GolgiResultReceiverEntry *blink, *flink;
    const char *msgId;
    GolgiResultReceiver *receiver;
};

class GolgiSendMsgImpl;

class GolgiAPIImpl {
public:
    static GolgiAPIImpl *getInstance(void);

    GolgiAPIImpl(GolgiNetInterface *netIf, const char *appKey, const char *devKey, const char *instId);
    void incomingRequest(GolgiMessage *msg);
    void incomingResponse(GolgiMessage *msg);
    void incomingError(GolgiMessage *msg);
    void processResult(GolgiMessage *msg, bool isErr);


    void sendResult(NTLResponseHandler *respHandler, GolgiMessage *msg);
    void sendRequest(NTLResponseHandler *respHandler, GolgiMessage *msg);

    void registerReceiver(const char *method, GolgiRequestReceiver *requestReceiver);
    void sendGolgiRequest(GolgiResultReceiver *resultReceiver, const char *dst, const char *method, const char *payload);
    void service(void);
    virtual ~GolgiAPIImpl();

private:
    void sendReqRes(NTLResponseHandler *respHandler, GolgiMessage *msg, bool isResult);
    NTLConn *ntlConn;
    GolgiSendMsgImpl *golgiSendMsgImpl;
    struct GolgiRequestReceiverEntry *requestReceiverRoot;
    struct GolgiResultReceiverEntry *resultReceiverRoot;
    const char *appKey;
    const char *devKey;
    const char *instId;
};
# 3 ".libgolgi.h" 2
# 1 "GolgiDataBlob.h" 1
# 11 "GolgiDataBlob.h"
class GolgiDataBlob{
  public:
    GolgiDataBlob();
    GolgiDataBlob(unsigned char *data, int32_t len);
    GolgiDataBlob *dupe(void);
    ~GolgiDataBlob();
    unsigned char *getData();
    int32_t getLen();

  private:
    int32_t len;
    unsigned char *data;

};
# 4 ".libgolgi.h" 2

# 1 "GolgiPayload.h" 1
# 12 "GolgiPayload.h"
class GolgiPayload : public NTLPayload {
public:
    GolgiPayload();
    virtual ~GolgiPayload();
};
# 6 ".libgolgi.h" 2
#define TYPE_MALLOC(s) (typeof(s))malloc(sizeof(*s))
#define FREE(a) tbxFree((void *)a)
#define LLINSERTB4(root, elem) {(elem)->flink = root;(elem)->blink = (root)->blink; (elem)->flink->blink = elem; (elem)->blink->flink = elem;}
#define LLINSERT(root, elem) {(elem)->flink = (root)->flink;(elem)->blink = root; (elem)->flink->blink = elem; (elem)->blink->flink = elem;}
#define LLREMOVE(elem) {(elem)->blink->flink = (elem)->flink;(elem)->flink->blink = (elem)->blink;}
#define LLSELFLINK(root) {(root)->flink = (root)->blink = root;}
#endif
#ifndef _B64_H_
#define _B64_H_

//
// b64.c
//

extern int32_t b64_decode(unsigned char *dst,
                      const char *src);

extern void b64_encode(char *dst,
                       unsigned char *src,
                       int32_t len);

extern void b64_init(void);

#endif
/*
 * GolgiNetWifi.h
 *
 *  Created on: Mar 2, 2015
 *      Author: brian
 */

#ifndef GOLGINETWIFI_H_
#define GOLGINETWIFI_H_

#include "libgolgi.h"
#include "WiFi.h"

class GolgiNetWifi : public GolgiNetInterface {
public:
    GolgiNetWifi();
    bool connect(const char *host, int32_t port);
    bool connected(void);
    void write(const char *str);
    int32_t available(void);
    int32_t read(void);
    int32_t read(void *dst, int32_t len);
    bool packetized();
    bool needsNops();
    void sendReq(unsigned long tid, const char *data);
    void sendRes(unsigned long tid, const char *data);
    void service(void);
    void stop(void);
    ~GolgiNetWifi();
private:
    WiFiClient *client;
    bool wasConnected;
};

#endif /* GOLGINETWIFI_H_ */
