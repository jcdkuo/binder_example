/* -*- mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*-
   vi:ai:tabstop=8:shiftwidth=4:softtabstop=4:expandtab
*/

/*
 * Author: Gabriel Burca <gburca dash binder at ebixio dot com>
 *
 * Sample code for using binders in Android from C++
 *
 * The Demo service provides 3 operations: push(), alert(), add(). See
 * the IDemo class documentation to see what they do.
 *
 * Both the server and client code are included below.
 *
 * To view the log output:
 *      adb logcat -v time binder_demo:* *:S
 *
 * To run, create 2 adb shell sessions. In the first one run "binder" with no
 * arguments to start the service. In the second one run "binder N" where N is
 * an integer, to start a client that connects to the service and calls push(N),
 * alert(), and add(N, 5).
 */

#define LOG_TAG "binder_server_demo"

/* For relevant code see:
    frameworks/native/{include,libs}/binder/{IInterface,Parcel}.{h,cpp}
    system/core/include/utils/{Errors,RefBase}.h
 */

#include <stdlib.h>
#include <utils/RefBase.h>
#include <utils/Log.h>
#include <binder/TextOutput.h>
#include <binder/IInterface.h>
#include <binder/IBinder.h>
#include <binder/ProcessState.h>
#include <binder/IServiceManager.h>
#include <binder/IPCThreadState.h>

using namespace android;


#define INFO(...) \
    do { \
        printf(__VA_ARGS__); \
        printf("\n"); \
        ALOGD(__VA_ARGS__); \
    } while(0)

void assert_fail(const char *file, int line, const char *func, const char *expr)
{
    INFO("assertion failed at file %s, line %d, function %s:",
         file, line, func);
    INFO("%s", expr);
    abort();
}

#define ASSERT(e) \
    do { \
        if (!(e)) \
            assert_fail(__FILE__, __LINE__, __func__, #e); \
    } while(0)


// Where to print the parcel contents: aout, alog, aerr. alog doesn't seem to work.
#define PLOG aout


// Interface (our AIDL) - Shared by server and client
class IDemoServer : public IInterface
{

public:
    enum {
        ALERT = IBinder::FIRST_CALL_TRANSACTION,
        PUSH,
        ADD
    };


    virtual void        push(int32_t data)          = 0;     // Sends a user-provided value to the service
    virtual void        alert()                     = 0;    // Sends a fixed alert string to the service
    virtual int32_t     add(int32_t v1, int32_t v2) = 0;    // Requests the service to perform an addition and return the result


    DECLARE_META_INTERFACE(DemoServer);  // Expands to 5 lines below:

    //static const android::String16 descriptor;
    //static android::sp<IDemoServer> asInterface(const android::sp<android::IBinder>& obj);
    //virtual const android::String16& getInterfaceDescriptor() const;
    //IDemoServer();
    //virtual ~IDemoServer();

};


//IMPLEMENT_META_INTERFACE(DemoServer, "DemoServer");
// Macro above expands to code below. Doing it by hand so we can log ctor and destructor calls.


const android::String16 IDemoServer::descriptor("DemoServer");
const android::String16& IDemoServer::getInterfaceDescriptor() const
{
    return IDemoServer::descriptor;
}


IDemoServer::IDemoServer()
{
    ALOGD(">>> IDemoServer::IDemoServer() constructor");
}

IDemoServer::~IDemoServer()
{
    ALOGD(">>> IDemoServer::~IDemServero() destructor");
}

// End of macro expansion

// Server
class BnDemoServer : public BnInterface<IDemoServer>
{
    virtual status_t onTransact(uint32_t code, const Parcel& data, Parcel* reply, uint32_t flags = 0);
};

status_t BnDemoServer::onTransact(uint32_t code, const Parcel& data, Parcel* reply, uint32_t flags)
{
    ALOGD("BnDemoServer::onTransact(%i) %i", code, flags);
    data.checkInterface(this);
    data.print(PLOG);
    endl(PLOG);

    switch(code) {
    case ALERT: {
        alert();    // Ignoring the fixed alert string
        ALOGD("BnDemoServer::onTransact alert");
        return NO_ERROR;
    }
    break;
    case PUSH: {
        int32_t inData = data.readInt32();
        ALOGD("BnDemoServer::onTransact got (%i)", inData);
        push(inData);
        ASSERT(reply != 0);
        reply->print(PLOG);
        endl(PLOG);
        return NO_ERROR;
    }
    break;
    case ADD: {
        int32_t inV1 = data.readInt32();
        int32_t inV2 = data.readInt32();
        int32_t sum = add(inV1, inV2);

        ALOGD("BnDemoServer::onTransact add(%i, %i) = %i", inV1, inV2, sum);
        ASSERT(reply != 0);
        reply->print(PLOG);
        endl(PLOG);
        reply->writeInt32(sum);

        return NO_ERROR;

    }
    break;
    default:
        return BBinder::onTransact(code, data, reply, flags);
    }
}

class BinderDemoServer : public BnDemoServer
{
    virtual void push(int32_t data)
    {
        INFO(">>> >>> >>> virtual DemoServer::push(%i)", data);
    }
    virtual void alert()
    {
        INFO(">>> >>> >>> virtual DemoServer::alert()");
    }
    virtual int32_t add(int32_t v1, int32_t v2)
    {
        INFO(">>> >>> >>> virtual DemoServer::add(%i, %i)", v1, v2);
        return v1 + v2;
    }
};



int main(int argc, char **argv)
{

    (void)argv;

    if (argc == 1) {

        ALOGD(">>> Binder_Test start DemoServer service ...................");

        defaultServiceManager()->addService(String16("DemoServer"), new BinderDemoServer());
        android::ProcessState::self()->startThreadPool();

        ALOGD(">>> Binder_Test Server service is now ready");

        IPCThreadState::self()->joinThreadPool();

        ALOGD(">>> Binder_Test Server service thread joined");

    } else {

        ALOGD(">>> Binder_Test BinderDemoServer ERROR!");
        INFO(">>> Binder_Test BinderDemoServer ERROR!");

        return -1;

    }

    return 0;
}

