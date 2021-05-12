//
// Created by stephane bourque on 2021-05-09.
//

#include "uStorageService.h"

namespace uCentral::Storage {

    bool AddCallback(uCentral::Objects::Callback & C) {
        return Service::instance()->AddCallback(C);
    }

    bool AddOrUpdateCallback(uCentral::Objects::Callback & C) {
        return Service::instance()->AddOrUpdateCallback(C);
    }

    bool UpdateCallback(std::string & UUID, uCentral::Objects::Callback & C) {
        return Service::instance()->UpdateCallback(UUID, C);
    }

    bool DeleteCallback(std::string & UUID) {
        return Service::instance()->DeleteCallback(UUID);
    }

    bool GetCallback(std::string & UUID, uCentral::Objects::Callback & C) {
        return Service::instance()->GetCallback(UUID, C);
    }

    bool GetCallbacks(uint64_t From, uint64_t HowMany, std::vector<uCentral::Objects::Callback> & Callbacks) {
        return Service::instance()->GetCallbacks(From, HowMany, Callbacks);
    }

/*
         uuid:
          type: string
          format: uuid
        uri:
          type: string
        creator:
          type: string
        location:
          type: string
          format: uri
        token:
          type: string
        tokenType:
          type: string
        created:
          type: string
          format: 'date-time'
        expires:
          type: string
          format: 'date-time'

 */

    typedef Poco::Tuple<
            std::string,
            std::string,
            std::string,
            std::string,
            std::string,
            std::string,
            std::string,
            uint64_t,
            uint64_t
    >   CallbackRecordTuple;
    typedef std::vector<CallbackRecordTuple>  CallbackRecordList;

/*
                    "UUID VARCHAR(64) PRIMARY KEY, "
                    "URI TEXT, "
                    "Location VARCHAR(128),"
                    "Token TEXT,"
                    "TokenType VARCHAR(64), ",
                    "Creator VARCHAR(128), "
                    "Topics TEXT, "
                    "Created BIGINT, "
                    "Expires BIGINT",
 */

    bool Service::AddCallback(uCentral::Objects::Callback & C) {
        try {
            Poco::Data::Session     Sess = Pool_->get();
            Poco::Data::Statement   Insert(Sess);

            std::string st{"INSERT INTO Callbacks ("
                           "UUID, URI, Location, Token, TokenType, Creator, Topics, Created, Expires"
                           ") VALUES(?,?,?,?,?,?,?,?,?)"
            };

            Insert  << ConvertParams(st),
                        Poco::Data::Keywords::use(C.UUID),
                        Poco::Data::Keywords::use(C.URI),
                        Poco::Data::Keywords::use(C.Location),
                        Poco::Data::Keywords::use(C.Token),
                        Poco::Data::Keywords::use(C.TokenType),
                        Poco::Data::Keywords::use(C.Creator),
                        Poco::Data::Keywords::use(C.Topics),
                        Poco::Data::Keywords::use(C.Created),
                        Poco::Data::Keywords::use(C.Expires);
            Insert.execute();
            return true;
        } catch (const Poco::Exception &E) {
            Logger_.log(E);
        }
        return false;
    }

    bool Service::UpdateCallback(std::string & UUID, uCentral::Objects::Callback & C) {
        try {
            Poco::Data::Session Sess = Pool_->get();
            Poco::Data::Statement   Update(Sess);

            std::string st{"UPDATE Callbacks "
                            " SET URI=?, Location=?, Token=?, TokenType=?, Creator=?, Topics=?, Created=?, Expires=?"
                            " WHERE UUID=?"
            };

            Update  << ConvertParams(st),
                    Poco::Data::Keywords::use(C.URI),
                    Poco::Data::Keywords::use(C.Location),
                    Poco::Data::Keywords::use(C.Token),
                    Poco::Data::Keywords::use(C.TokenType),
                    Poco::Data::Keywords::use(C.Creator),
                    Poco::Data::Keywords::use(C.Topics),
                    Poco::Data::Keywords::use(C.Created),
                    Poco::Data::Keywords::use(C.Expires),
                    Poco::Data::Keywords::use(C.UUID);
            Update.execute();
            return true;

        } catch (const Poco::Exception &E) {
            Logger_.log(E);
        }
        return false;
    }

    bool Service::AddOrUpdateCallback(uCentral::Objects::Callback & C) {
        try {
            Poco::Data::Session     Sess = Pool_->get();
            Poco::Data::Statement   Select(Sess);

            std::string st{"SELECT UUID FROM Callbacks WHERE UUID=?"};
            std::string TmpUUID;
            Select <<   ConvertParams(st),
                        Poco::Data::Keywords::into(TmpUUID),
                        Poco::Data::Keywords::use(C.UUID);
            Select.execute();
            if(TmpUUID.empty()) {
                return AddCallback(C);
            }
            else {
                return UpdateCallback(TmpUUID, C);
            }
        }  catch (const Poco::Exception &E) {
            Logger_.log(E);
        }
        return false;
    }

    bool Service::DeleteCallback(std::string & UUID) {
        try {
            Poco::Data::Session     Sess = Pool_->get();
            Poco::Data::Statement   Delete(Sess);

            std::string st{"DELETE FROM Callbacks WHERE UUID=?"};

            Delete <<   ConvertParams(st),
                        Poco::Data::Keywords::use(UUID);
            Delete.execute();

            return true;
        } catch (const Poco::Exception &E) {
            Logger_.log(E);
        }
        return false;
    }

    bool Service::GetCallback(std::string & UUID, uCentral::Objects::Callback & C) {
        try {
            Poco::Data::Session     Sess = Pool_->get();
            Poco::Data::Statement   Select(Sess);

            std::string st{"SELECT "
                           "UUID, URI, Location, Token, TokenType, Creator, Topics, Created, Expires"
                           " FROM Callbacks WHERE UUID=?"
            };
            Select  << ConvertParams(st),
                    Poco::Data::Keywords::into(C.UUID),
                    Poco::Data::Keywords::into(C.URI),
                    Poco::Data::Keywords::into(C.Location),
                    Poco::Data::Keywords::into(C.Token),
                    Poco::Data::Keywords::into(C.TokenType),
                    Poco::Data::Keywords::into(C.Creator),
                    Poco::Data::Keywords::into(C.Topics),
                    Poco::Data::Keywords::into(C.Created),
                    Poco::Data::Keywords::into(C.Expires),
                    Poco::Data::Keywords::use(UUID);
            Select.execute();
            return true;
        } catch (const Poco::Exception &E) {
            Logger_.log(E);
        }
        return false;
    }

    bool Service::GetCallbacks(uint64_t From, uint64_t HowMany, std::vector<uCentral::Objects::Callback> & Callbacks) {
        try {
            CallbackRecordList  Records;
            Poco::Data::Session Sess = Pool_->get();
            Poco::Data::Statement   Select(Sess);

            std::string st{ "SELECT "
                                " UUID, URI, Location, Token, TokenType, Creator, Topics, Created, Expires"
                                " FROM Callbacks"
            };
            Select  << ConvertParams(st),
                        Poco::Data::Keywords::into(Records),
                        Poco::Data::Keywords::range(From,From+HowMany);
            Select.execute();

            for(const auto &i:Records) {
                uCentral::Objects::Callback C{
                    .UUID = i.get<0>(),
                    .URI = i.get<1>(),
                    .Location = i.get<2>(),
                    .Token = i.get<3>(),
                    .TokenType = i.get<4>(),
                    .Creator = i.get<5>(),
                    .Topics = i.get<6>(),
                    .Created = i.get<7>(),
                    .Expires = i.get<8>()
                };

                Callbacks.push_back(C);
            }

            return true;

        } catch (const Poco::Exception &E) {
            Logger_.log(E);
        }
        return false;
    }
}
