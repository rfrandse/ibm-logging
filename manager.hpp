#pragma once

#include <experimental/any>
#include <experimental/filesystem>
#include <map>
#include <sdbusplus/bus.hpp>
#include "dbus.hpp"
#include "interfaces.hpp"

namespace ibm
{
namespace logging
{


/**
 * @class Manager
 *
 * This class hosts IBM specific interfaces for the error logging
 * entry objects.  It watches for interfaces added and removed
 * signals to know when to create and delete objects.  Handling the
 * xyz.openbmc_project.Logging service going away is done at the
 * systemd service level where this app will be stopped too.
 */
class Manager
{
    public:

        Manager() = delete;
        ~Manager() = default;
        Manager(const Manager&) = delete;
        Manager& operator=(const Manager&) = delete;
        Manager(Manager&&) = delete;
        Manager& operator=(Manager&&) = delete;

        /**
         * Constructor
         *
         * @param[in] bus - the D-Bus bus object
         */
        explicit Manager(sdbusplus::bus::bus& bus);

    private:

        /**
         * The callback for an interfaces added signal
         *
         * Creates the IBM interfaces for the log entry
         * that was just created.
         *
         * @param[in] msg - the sdbusplus message
         */
        void interfaceAdded(sdbusplus::message::message& msg);

        /**
         * The callback for an interfaces removed signal
         *
         * Removes the IBM interfaces for the log entry
         * that was just removed.
         *
         * @param[in] msg - the sdbusplus message
         */
        void interfaceRemoved(sdbusplus::message::message& msg);

        /**
         * Returns the entry ID for a log
         *
         * @param[in] objectPath - the object path of the log
         *
         * @return uint32_t - the ID
         */
        inline uint32_t getEntryID(const std::string& objectPath)
        {
            std::experimental::filesystem::path path(objectPath);
            return std::stoul(path.filename());
        }

        /**
         * The sdbusplus bus object
         */
        sdbusplus::bus::bus& bus;

        /**
         * The match object for interfacesAdded
         */
        sdbusplus::bus::match_t addMatch;

        /**
         * The match object for interfacesRemoved
         */
        sdbusplus::bus::match_t removeMatch;

        using EntryID = uint32_t;
        using InterfaceMap = std::map<InterfaceType, std::experimental::any>;
        using EntryMap = std::map<EntryID, InterfaceMap>;

        /**
         * A map of the error log IDs to their IBM interface objects.
         * There may be multiple interfaces per ID.
         */
        EntryMap entries;
};

}
}
