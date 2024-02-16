#define BOT_TOKEN ""
#define RED_PIN 23
const char* ssid = "";
const char* password = "";

// Timeout for the bot
int bot_lasttime = 0;
const unsigned long BOT_MTBS = 1000;

// Timeout for the water
int water_lasttime = 0;
const unsigned long WATER_MTBS = 60000;

WiFiClientSecure client;
UniversalTelegramBot bot(BOT_TOKEN, client);

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 3600;
const int   daylightOffset_sec = 3600;

const int startSpring = 221;
const int startSummer = 521;
const int startAutumn = 821;
const int startWinter = 1121;
const int waterIntSpring = 7;
const int waterIntSummer = 3;
const int waterIntAutumn = 10;
const int waterIntWinter = 14;
