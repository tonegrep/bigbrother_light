#define HOST_TRANSMIT "http://192.168.1.10:8000/hardware/transmit"
#define HOST_STATUS_REPORT "http://192.168.1.10:8000/hardware/status"

#define STATUS_READY   0
#define STATUS_BUSY    1
#define STATUS_OFFLINE 2


#define JOB_CLEAR                100
#define STATUS_CHANGED           101
#define ERROR_PIN_WRITE          200
#define ERROR_PIN_READ           201
#define ERROR_BAD_REQUEST        202
#define ERROR_SERVER_UNAVAILABLE 203
#define ERROR_CLIENT_UNAVAILABLE 204
#define ERROR_BUSY               205

#define CONTROLLER_TYPE_LIGHT    0
#define CONTROLLER_TYPE_SENSOR   1
#define CONTROLLER_TYPE_RC       2

typedef struct {
    String UUID;
    int    status_code;
    int    response_code;
    int    data;
} json_info_t;

#define HTML_INFO(str, name, x) str = "<!DOCTYPE HTML>\r\n<html>\r\n"; \
  str += "<h3>current "; \
  str += name; \
  str += " is "; \
  str += x; \
  str += "</h3>"; \
  str += "</html>\n";


#define HTML_INFO_CHANGABLE(str, name, x) str = "<!DOCTYPE HTML>\r\n<html>\r\n"; \
  str += "<h3>current "; \
  str += name; \
  str += " is "; \
  str += x; \
  str += "</h3>"; \
  str += "<form action=\"SET\" method=\"post\">"; \
  str += "<h5> Change this: </h5>"; \
  str += "<input name=\"brightness\">"; \
  str += "<button type=\"submit\" value=\"SET\"> Set </button>"; \
  str += "</html>\n";
  
  

