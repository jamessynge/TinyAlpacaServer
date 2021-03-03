#ifdef ARDUINO
#include <Arduino.h>
#include <TinyAlpacaServer.h>
#else
#include "TinyAlpacaServer.h"
#endif

using alpaca::Literal;

constexpr char buffer1[] PROGMEM = "abc";

constexpr Literal literal(buffer1);

void setup() {
  // Setup serial, wait for it to be ready so that our logging messages can be
  // read.
  Serial.begin(9600);
  // Wait for serial port to connect, or at least some minimum amount of time
  // (TBD), else the initial output gets lost.
  while (!Serial) {
  }
}

int test_dense_switch_map() {
  // This kind of switch statement, acting as a simple, dense map
  // of N to M ends up in RAM, presumably as a table of values
  // to be returned.
  switch (millis() % 100) {
    case 0:
      return 27393;
    case 1:
      return 3768;
    case 2:
      return 25399;
    case 3:
      return 4762;
    case 4:
      return 188;
    case 5:
      return 18234;
    case 6:
      return 12875;
    case 7:
      return 27792;
    case 8:
      return 31214;
    case 9:
      return 24545;
    case 10:
      return 12815;
    case 11:
      return 30294;
    case 12:
      return 27676;
    case 13:
      return 18710;
    case 14:
      return 5111;
    case 15:
      return 30766;
    case 16:
      return 19014;
    case 17:
      return 17420;
    case 18:
      return 24503;
    case 19:
      return 30618;
    case 20:
      return 7986;
    case 21:
      return 27869;
    case 22:
      return 17386;
    case 23:
      return 32366;
    case 24:
      return 28652;
    case 25:
      return 20491;
    case 26:
      return 1633;
    case 27:
      return 23332;
    case 28:
      return 19818;
    case 29:
      return 7924;
    case 30:
      return 3671;
    case 31:
      return 23503;
    case 32:
      return 15320;
    case 33:
      return 22000;
    case 34:
      return 14929;
    case 35:
      return 25922;
    case 36:
      return 32746;
    case 37:
      return 24829;
    case 38:
      return 5958;
    case 39:
      return 27355;
    case 40:
      return 23935;
    case 41:
      return 26890;
    case 42:
      return 17534;
    case 43:
      return 29878;
    case 44:
      return 10605;

    default:
      return 20;
  }
}

unsigned int test_dense_switch_to_calculations() {
  auto milliseconds = millis();
  auto microseconds = micros();
  // This kind of switch statement, where each case requires execution of code,
  // does not end up as a data table.
  switch (milliseconds % 100) {
    case 0:
      return (microseconds * 206) % 14673;
    case 1:
      return (microseconds * 484) % 11622;
    case 2:
      return (microseconds * 566) % 10004;
    case 3:
      return (microseconds * 39) % 17684;
    case 4:
      return (microseconds * 415) % 13548;
    case 5:
      return (microseconds * 505) % 14766;
    case 6:
      return (microseconds * 267) % 15776;
    case 7:
      return (microseconds * 450) % 15454;
    case 8:
      return (microseconds * 287) % 19511;
    case 9:
      return (microseconds * 746) % 19804;
    case 10:
      return (microseconds * 769) % 12029;
    case 11:
      return (microseconds * 667) % 18826;
    case 12:
      return (microseconds * 776) % 11443;
    case 13:
      return (microseconds * 246) % 18968;
    case 14:
      return (microseconds * 258) % 17492;
    case 15:
      return (microseconds * 22) % 11050;
    case 16:
      return (microseconds * 306) % 13526;
    case 17:
      return (microseconds * 26) % 12475;
    case 18:
      return (microseconds * 397) % 14906;
    case 19:
      return (microseconds * 953) % 16879;
    case 20:
      return (microseconds * 393) % 18279;
    case 21:
      return (microseconds * 328) % 18315;
    case 22:
      return (microseconds * 683) % 18695;
    case 23:
      return (microseconds * 247) % 11547;
    case 24:
      return (microseconds * 407) % 11957;
    case 25:
      return (microseconds * 915) % 12939;
    case 26:
      return (microseconds * 437) % 11218;
    case 27:
      return (microseconds * 201) % 18743;
    case 28:
      return (microseconds * 117) % 14758;
    case 29:
      return (microseconds * 347) % 12536;
    case 30:
      return (microseconds * 940) % 10586;
    case 31:
      return (microseconds * 643) % 15152;
    case 32:
      return (microseconds * 614) % 12115;
    case 33:
      return (microseconds * 674) % 15951;
    case 34:
      return (microseconds * 172) % 16101;
    case 35:
      return (microseconds * 957) % 18410;
    case 36:
      return (microseconds * 788) % 19195;
    case 37:
      return (microseconds * 960) % 19706;
    case 38:
      return (microseconds * 633) % 13285;
    case 39:
      return (microseconds * 717) % 15157;
    case 40:
      return (microseconds * 832) % 17748;
    case 41:
      return (microseconds * 304) % 18109;
    case 42:
      return (microseconds * 508) % 14911;
    case 43:
      return (microseconds * 84) % 19512;
    case 44:
      return (microseconds * 222) % 15241;
    case 45:
      return (microseconds * 780) % 18022;
    case 46:
      return (microseconds * 74) % 19502;
    case 47:
      return (microseconds * 727) % 17775;
    case 48:
      return (microseconds * 510) % 13540;
    case 49:
      return (microseconds * 68) % 17984;
    default:
      return 20;
  }
}

long test_sparse_switch_map() {
  long microseconds = micros();
  // This kind of switch statement, where the case values are sparse,
  // does not end up as a data table.
  switch (microseconds) {
    case 3319:
      return 4781;
    case 5228:
      return 13832;
    case 6553:
      return 12047;
    case 6762:
      return 25980;
    case 9025:
      return 30839;
    case 10228:
      return 18066;
    case 10423:
      return 10333;
    case 16000:
      return 11491;
    case 17167:
      return 8279;
    case 17945:
      return 19118;
    case 19363:
      return 22645;
    case 23622:
      return 16126;
    case 24819:
      return 16863;
    case 25643:
      return 10609;
    case 27102:
      return 7164;
    case 27915:
      return 8174;
    case 31325:
      return 3987;
    case 34483:
      return 26985;
    case 37192:
      return 12588;
    case 37370:
      return 23696;
    case 52055:
      return 13453;
    case 53489:
      return 11080;
    case 54856:
      return 22181;
    case 55983:
      return 11246;
    case 60082:
      return 22607;
    case 60328:
      return 3666;
    case 61222:
      return 8562;
    case 66655:
      return 21223;
    case 69076:
      return 31115;
    case 69285:
      return 19931;
    case 75098:
      return 19411;
    case 76167:
      return 12718;
    case 80898:
      return 29351;
    case 82847:
      return 9629;
    case 83591:
      return 31709;
    case 84301:
      return 19301;
    case 88882:
      return 2072;
    case 89162:
      return 3370;
    case 89303:
      return 31745;
    case 91987:
      return 4430;
    case 94534:
      return 21572;
    case 95225:
      return 4774;
    case 96647:
      return 14672;
    case 97452:
      return 4437;
    case 99932:
      return 19091;
    default:
      return 123;
  }
}

int test_if_sequence() {
  // This does not get turned into a table in RAM.
  auto t = millis() % 100;
  if (t == 0) return 16695;
  if (t == 1) return 12605;
  if (t == 2) return 5368;
  if (t == 3) return 16481;
  if (t == 4) return 6905;
  if (t == 5) return 11514;
  if (t == 6) return 6407;
  if (t == 7) return 14497;
  if (t == 8) return 27701;
  if (t == 9) return 31136;
  if (t == 10) return 31015;
  if (t == 11) return 16878;
  if (t == 12) return 21520;
  if (t == 13) return 21910;
  if (t == 14) return 31947;
  if (t == 15) return 3214;
  if (t == 16) return 15712;
  if (t == 17) return 25585;
  if (t == 18) return 16191;
  if (t == 19) return 31778;
  if (t == 20) return 16202;
  if (t == 21) return 32283;
  if (t == 22) return 26165;
  if (t == 23) return 25569;
  if (t == 24) return 32690;
  if (t == 25) return 4854;
  if (t == 26) return 23180;
  if (t == 27) return 12784;
  if (t == 28) return 1013;
  if (t == 29) return 15124;
  if (t == 30) return 23714;
  if (t == 31) return 757;
  if (t == 32) return 8148;
  if (t == 33) return 10121;
  if (t == 34) return 7490;
  if (t == 35) return 441;
  if (t == 36) return 31863;
  if (t == 37) return 25239;
  if (t == 38) return 21108;
  if (t == 39) return 22654;
  if (t == 40) return 5280;
  if (t == 41) return 15469;
  if (t == 42) return 15211;
  if (t == 43) return 14598;
  if (t == 44) return 26449;
  return t;
}

void loop() {
  delay(1000);
  Serial.print("test method returned: ");
  Serial.println(test_sparse_switch_map());

  Serial.print("literal: ");
  literal.printTo(Serial);
  Serial.println();
  Serial.println();

  Serial.print("JSON literal: ");
  literal.printJsonEscapedTo(Serial);
  Serial.println();
  Serial.println();
}
