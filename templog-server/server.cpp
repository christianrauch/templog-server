#include <SensorDatabase/SensorDatabase.h>
#include <TempReader/TempReader.h>
#include <pthread.h>
#include <iostream>
#include "Timer.h"
#include "logger.h"
#include <glib.h>

#include <CommunicationService/SensorLoggerCommunicationServer.h>

#ifdef DEBUG
    #define CONFPATH "../templog.conf"
#else
    #define CONFPATH "/etc/templog.conf"
#endif

void dbg_db_fake(SensorDatabase &db) {
    db.reset();
    std::string image_data = "abcdef";
    db.addSetting("basement", image_data.data(), image_data.size());
    db.addSensor("pipe", "67C6697351FF", "28", "DS18B20", "°C", "basement");
    db.addSensor("oven", "4AEC29CDBAAB", "10", "DS18B20", "°C", "basement");
    db.addSensor("watertank", "F2FBE3467CC2", "10", "DS18B20", "°C", "basement");
}

void dbg_reader_fake(TempReader &reader) {
    reader.addFake("10");
    reader.addFake("10");
    reader.addFake("28");
}

int main() {
    std::cout<<"reading config from: "<<CONFPATH<<std::endl;
    // read configuration
    GKeyFile *conf_file = g_key_file_new ();
    GKeyFileFlags conffile_flags;
    GError *conffile_error = NULL;

    if(!g_key_file_load_from_file(conf_file, CONFPATH, conffile_flags, &conffile_error)) {
        g_error(conffile_error->message);
    }

    const char *db_path = g_key_file_get_string(conf_file, "database", "db_file", &conffile_error);
    const time_t period = g_key_file_get_integer(conf_file, "logging", "period", &conffile_error);
    const unsigned int port = g_key_file_get_integer(conf_file, "communication", "port", &conffile_error);
    const char *device = g_key_file_get_string(conf_file, "logging", "device", &conffile_error);

    std::cout<<"db path: "<<std::string(db_path)<<std::endl;
    std::cout<<"period: "<<period<<std::endl;

    if(db_path == NULL)
        g_error(conffile_error->message);

    SensorDatabase db;
    TempReader reader;

    db.open(db_path);

    std::cout<<"addr db: "<<&db<<std::endl;
    std::cout<<"addr reader: "<<&reader<<std::endl;

    if(std::string(device)=="fake") {
        // init with example data
        std::cout<<"feeding fake data"<<std::endl;
        dbg_db_fake(db);
        dbg_reader_fake(reader);
    }
    else {
        reader.setDevice(device);
    }

    reader.open();

    LoggerConfig conf = {.period=period, .db=&db, .reader=&reader};

    pthread_t th_logger;

    std::cout<<"addr per: "<<&period<<std::endl;
    std::cout<<"addr conf: "<<&conf<<std::endl;

    //pthread_create(&th_logger, NULL, logger, (void*)period);
    pthread_create(&th_logger, NULL, logger, &conf);
    //pthread_create(&th_logger, NULL, logger, NULL);

    SensorLoggerCommunicationServer s(port, db, reader);

    s.start();

    pthread_join(th_logger, NULL);

    std::cout<<"exit..."<<std::endl;
}
