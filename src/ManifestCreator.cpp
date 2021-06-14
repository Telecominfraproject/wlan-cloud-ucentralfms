//
// Created by stephane bourque on 2021-06-02.
//

#include "Poco/JSON/Object.h"
#include "Poco/JSON/Parser.h"
#include "Poco/JSON/Stringifier.h"

#include "ManifestCreator.h"
#include "Utils.h"

namespace uCentral {
    class ManifestCreator *ManifestCreator::instance_ = nullptr;

    ManifestCreator::ManifestCreator() noexcept:
            SubSystemServer("ManifestCreator", "MANIFEST-MGR", "manifestcreator") {
    }

    void ManifestCreator::run() {
        Running_ = true;

        uCentral::S3BucketReader BR;
        BR.Initialize();

        while(Running_) {
            Poco::Thread::trySleep(60000);
            if(!Running_)
                break;
            uCentral::BucketContent BucketList;
            BR.ReadBucket();
            BucketList = BR.Bucket();
            // ComputeManifest(BucketList);
            Print(BucketList);
        }
    }

    bool ManifestCreator::ComputeManifest(uCentral::BucketContent &BucketContent) {

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

    int ManifestCreator::Start() {
        BucketReader_.Initialize();
        Worker_.start(*this);
        return 0;
    }

    void ManifestCreator::Stop() {
        Running_ = false;
        Worker_.wakeUp();
        Worker_.join();
    }

    bool ManifestCreator::Update() {
        Worker_.wakeUp();
        return true;
    }
}
