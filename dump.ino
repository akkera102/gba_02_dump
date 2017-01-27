const int WR  = 2;
const int RD  = 3;
const int CS  = 4;
const int CS2 = 5;
const int AD0 = 22;

void setup() 
{
  pinMode(CS, OUTPUT);
  digitalWrite(CS, LOW);

  pinMode(RD, OUTPUT);
  digitalWrite(RD, HIGH);

  pinMode(WR, OUTPUT);
  digitalWrite(WR, HIGH);

  pinMode(CS2, OUTPUT);
  digitalWrite(CS2, HIGH);

  for(int i=0; i<24; i++)
  {
    pinMode(AD0+i, OUTPUT);
  }

  Serial.begin(115200);

/*
  Serial.println(getHeaderName() + " " + getHeaderCode() + " 0x" + String(getRomSize(), HEX));

  for(int i=0; i<0x16; i++)
  {
    word data = read16(i);

    Serial.print(i, HEX);
    Serial.print(" : ");
    Serial.print(data, HEX);
    Serial.println("");
  }
  for(;;){}
*/
  dumpROM();
}

void loop() 
{
  // EMPTY
}

void dumpROM()
{
  unsigned long size = getRomSize();
  send8(size >> 24);
  send8(size >> 16);
  send8(size >>  8);
  send8(size);

  for(unsigned long i=0; i<size/2; i++)
  {
    word data = read16(i);
    send16(data);
  }

  Serial.println("Dump completed!");
  Serial.end();

  for(;;){}
}

void setAdr(unsigned long adr)
{
  for(int i=0; i<24; i++)
  {
    pinMode(AD0+i, OUTPUT);
    digitalWrite(AD0+i, (adr & ((unsigned long)1 << i)) ? HIGH : LOW);
  }

  digitalWrite(CS, HIGH);
  delayMicroseconds(10);
  digitalWrite(CS, LOW);

  for(int i=0; i<24; i++)
  {
    digitalWrite(AD0+i, LOW);
  }
}

byte read8(unsigned long adr)
{
  word ret = read16(adr / 2);

  if(adr & 0x1 == 1)
  {
    return ret >> 8;
  }
  else
  {
    return ret & 0xff;
  }
}

word read16(unsigned long adr)
{
  setAdr(adr);

  for(int i=0; i<16; i++)
  {
    pinMode(AD0+i, INPUT);
  }

  digitalWrite(RD, LOW);
  delayMicroseconds(10);
  digitalWrite(RD, HIGH);

  word ret = 0;
  
  for(int i=0; i<16; i++)
  {
    if(digitalRead(AD0+i) == HIGH)
    {
      ret |= (1 << i);
    }
  }

  return ret;
}

void send8(byte data)
{
  Serial.write(data);
}

void send16(word data)
{
  Serial.write((data & 0x00ff)     );
  Serial.write((data & 0xff00) >> 8);
}

String getHeaderName()
{
  String ret;
  char c;

  for(int i=0; i<12; i++)
  {
    c = (char)read8(0xa0+i);

    if(c == '\0')
    {
      break;
    }

    ret.concat(c);
  }

  return ret;
}

String getHeaderCode()
{
  String ret;
  char c;

  for(int i=0; i<4; i++)
  {
    c = (char)read8(0xac+i);
    ret.concat(c);
  }

  return ret;
}

unsigned long getRomSize()
{
  unsigned long ret = 0x02000000;

  while(ret != 0x0)
  {
    ret -= 0x1000;

    if(read8(ret) != 0x0)
    {
      break;
    }
  }

  ret = ((ret + 0xfffff) & 0xfff00000);

  return ret;
}

