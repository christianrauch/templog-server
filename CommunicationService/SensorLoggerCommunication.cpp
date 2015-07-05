#include "SensorLoggerCommunication.h"
#include <SensorDatabase/SensorDatabase.h>
#include <TempReader/TempReader.h>
#include <TempReader/Sensor.h>

using namespace  ::communication;

class SensorLoggerCommunicationHandler : virtual public SensorLoggerCommunicationIf {
private:
    SensorDatabase *db = NULL;
    TempReader *reader = NULL;

public:
    SensorLoggerCommunicationHandler(SensorDatabase &db, TempReader &reader){
        std::cout<<"db Handle: "<<&db<<std::endl;
        this->db = &db;
        this->reader = &reader;
        std::cout<<"db Handle (this): "<<this->db<<std::endl;
        std::cout<<"reader Handle (this): "<<this->reader<<std::endl;
    }

    double getSensorValue(const std::string& name) {
        // Your implementation goes here
        printf("getSensorValue\n");
        std::cout<<"db: "<<db<<std::endl;
    }

    void getSensorRange(std::map<int64_t, double> & _return, const std::string& name, const int64_t t_start, const int64_t t_end) {
        // Your implementation goes here
        printf("getSensorRange\n");
    }

    void addSetting(const std::string& name, const std::string& image) {
        // Your implementation goes here
        printf("addSetting\n");
        db->addSetting(name);
    }

    void addSensor(const std::string& name, const std::string& id, const std::string& family, const std::string& type, const std::string& unit, const std::string& setting, const std::vector<double> & position) {
        // Your implementation goes here
        printf("addSensor\n");
        db->addSensor(name, id, family, type, unit, setting, position);
    }

    void getSensorOnline(std::vector< ::communication::Sensor > &_return) {
        // Your implementation goes here
        printf("getSensorOnline\n");
        const std::vector< ::Sensor > sensors_ow = reader->getSensors();

        // copy data between different Sensor classes
        for(auto s_ow: sensors_ow) {
            ::communication::Sensor s;
            s.family = s_ow.family;
            s.id = s_ow.id;
            s.type = s_ow.type;
            s.time = s_ow.time;
            s.value = s_ow.value;
            s.unit = s_ow.unit;
            _return.push_back(s);
        }
    }

};
