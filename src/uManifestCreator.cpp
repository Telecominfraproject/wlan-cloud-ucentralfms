//
// Created by stephane bourque on 2021-06-02.
//

#include "Poco/JSON/Object.h"
#include "Poco/JSON/Parser.h"
#include "Poco/JSON/Stringifier.h"

#include "uManifestCreator.h"
#include "uUtils.h"

namespace uCentral::ManifestCreator {
    Service *Service::instance_ = nullptr;

    int Start() {
        return Service::instance()->Start();
    }

    void Stop() {
        Service::instance()->Stop();
    }

    bool Update() {
        return Service::instance()->Update();
    }

    Service::Service() noexcept:
            uSubSystemServer("ManifestCreator", "MANIFEST-MGR", "manifestcreator") {
    }

    void Service::run() {
        Running_ = true;

        uCentral::S3BucketReader BR;
        DBGLINE
        BR.Initialize();
        DBGLINE

        while(Running_) {
            Poco::Thread::trySleep(10000);
            DBGLINE
            if(!Running_)
                break;
            DBGLINE
            uCentral::BucketContent BucketList;
            DBGLINE
            BR.ReadBucket();
            DBGLINE
            BucketList = BR.Bucket();
            DBGLINE
            ComputeManifest(BucketList);
            DBGLINE
            Print(BucketList);
            DBGLINE
        }
    }

    bool Service::ComputeManifest(uCentral::BucketContent &BucketContent) {

        for(auto &[Name,Entry]:BucketContent) {
            try {
                Poco::JSON::Parser  P;
                auto ParsedContent = P.parse(Entry.S3ContentManifest).extract<Poco::JSON::Object::Ptr>();

                if(ParsedContent->has("image") &&
                    ParsedContent->has("compatible") &&
                    ParsedContent->has("revision") &&
                    ParsedContent->has("timestamp"))
                {
                    Entry.Timestamp = ParsedContent->get("timestamp");
                    Entry.Compatible = ParsedContent->get("compatible").toString();
                    Entry.Revision = ParsedContent->get("revision").toString();
                    Entry.Image = ParsedContent->get("image").toString();
                    if(Entry.Image!=Name) {
                        Logger_.error(Poco::format("MANIFEST(%s): Image name does not match manifest name (%s).",Name,Entry.Image));
                        continue;
                    }
                    Entry.Valid = true;

                } else {
                    Logger_.error(Poco::format("MANIFEST(%s): Entry does not have a valid JSON manifest.",Name));
                }
            } catch (const Poco::Exception  &E ) {
                Logger_.log(E);
            }
        }
        return true;
    }

    int Service::Start() {
        BucketReader_.Initialize();

        Worker_.start(*this);

        return 0;
    }

    void Service::Stop() {
        DBGLINE
        Running_ = false;
        DBGLINE
        Worker_.wakeUp();
        DBGLINE
        Worker_.join();
        DBGLINE
    }

    bool Service::Update() {
        Worker_.wakeUp();
        return true;
    }
}
