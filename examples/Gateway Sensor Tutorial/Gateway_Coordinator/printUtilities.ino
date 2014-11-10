
uint8_t const MONTHS[12] = {
  31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

#define IS_LEAPYEAR(x) (((x%4==0)&&!(x%100==0))||(x%400==0))
#define STARTYEAR	1970

void datePrint(Print& p, uint32_t timeval){
  uint16_t days=timeval/86400;
  if (timeval>1400000000){
    uint16_t years=timeval/31536000;
    uint16_t daysThisYear=0;
    for (int year=0;year<years;year++){
      daysThisYear+=(IS_LEAPYEAR(year))?366:365;
    }
    years+=STARTYEAR;
    daysThisYear=days-daysThisYear;
    days=0;
    uint8_t months;

    for (months=0;days<daysThisYear;months++){
      days+=MONTHS[months];
    }
    days=daysThisYear-(days-MONTHS[months-1])+1;

    p.print(years);
    p.print("-");
    p.print(months);
    p.print("-");
    p.print(days);
    //#endif
  } 
  else{
    p.print(days);
  }
}

void timePrint(Print& p, uint32_t timeval){
  uint8_t hours=	(timeval%86400)/3600;
  uint8_t minutes=(timeval%3600)/60;
  uint8_t seconds= timeval%60;
  if (hours<10) p.print("0");
  p.print(hours);
  p.print(":");
  if (minutes<10) p.print("0");
  p.print(minutes);
  p.print(":");
  if (seconds<10) p.print("0");
  p.print(seconds);
}


