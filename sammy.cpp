#include "daisysp.h"
#include "daisy_patch.h"

#include "sam/reciter.h"
#include "sam/sam.h" // https://github.com/vidarh/SAM

using namespace daisy;
using namespace daisysp;

DaisyPatch patch;

Parameter playbackSpeed, attenuator, cutoffLow, repeat;

Svf filter, filterPost;
WhiteNoise noise;

//  Written in 2019 by David Blackman and Sebastiano Vigna (vigna@acm.org)
static uint64_t srnd[4];

static inline uint64_t rotl(const uint64_t x, int k) {
	return (x << k) | (x >> (64 - k));
}

uint64_t nextRnd(void) {
	const uint64_t result = rotl(srnd[0] + srnd[3], 23) + srnd[0];

	const uint64_t t = srnd[1] << 17;

	srnd[2] ^= srnd[0];
	srnd[3] ^= srnd[1];
	srnd[1] ^= srnd[2];
	srnd[0] ^= srnd[3];

	srnd[2] ^= t;

	srnd[3] = rotl(srnd[3], 45);

	return result;
}
// ------

#define FADE_LEN 1024.f

#define N_WORD_CHARS 5966
char words[N_WORD_CHARS] = {",A,ABOUT,ABOVE,ACROSS,ACT,ACTIVE,ACTIVITY,ADD,AFRAID,AFTER,AGAIN,AGE,AGO,AGREE,AIR,ALL,ALONE,ALONG,ALREADY,ALWAYS,AM,AMOUNT,AN,AND,ANGRY,ANOTHER,ANSWER,ANY,ANYONE,ANYTHING,ANYTIME,APPEAR,APPLE,ARE,AREA,ARM,ARMY,AROUND,ARRIVE,ART,AS,ASK,AT,ATTACK,AUNT,AUTUMN,AWAY,BABY,BASE,BACK,BAD,BAG,BALL,BANK,BASKET,BATH,BE,BEAN,BEAR,BEAUTIFUL,BEER,BED,BEDROOM,BEHAVE,BEFORE,BEGIN,BEHIND,BELL,BELOW,BESIDES,BEST,BETTER,BETWEEN,BIG,BIRD,BIRTH,BIRTHDAY,BIT,BITE,BLACK,BLEED,BLOCK,BLOOD,BLOW,BLUE,BOARD,BOAT,BODY,BOIL,BONE,BOOK,BORDER,BORN,BORROW,BOTH,BOTTLE,BOTTOM,BOWL,BOX,BOY,BRANCH,BRAVE,BREAD,BREAK,BREAKFAST,BREATHE,BRIDGE,BRIGHT,BRING,BROTHER,BROWN,BRUSH,BUILD,BURN,BUSINESS,BUS,BUSY,BUT,BUY,BY,CAKE,CALL,CAN,CANDLE,CAP,CAR,CARD,CARE,CAREFUL,CARELESS,CARRY,CASE,CAT,CATCH,CENTRAL,CENTURY,CERTAIN,CHAIR,CHANCE,CHANGE,CHASE,CHEAP,CHEESE,CHICKEN,CHILD,CHILDREN,CHOCOLATE,CHOICE,CHOOSE,CIRCLE,CITY,CLASS,CLEVER,CLEAN,CLEAR,CLIMB,CLOCK,CLOTH,CLOTHES,CLOUD,CLOUDY,CLOSE,COFFEE,COAT,COIN,COLD,COLLECT,COLOUR,COMB,COMFORTABLE,COMMON,COMPARE,COME,COMPLETE,COMPUTER,CONDITION,CONTINUE,CONTROL,COOK,COOL,COPPER,CORN,CORNER,CORRECT,COST,CONTAIN,COUNT,COUNTRY,COURSE,COVER,CRASH,CROSS,CRY,CUP,CUPBOARD,CUT,DANCE,DANGEROUS,DARK,DAUGHTER,DAY,DEAD,DECIDE,DECREASE,DEEP,DEER,DEPEND,DESK,DESTROY,DEVELOP,DIE,DIFFERENT,DIFFICULT,DINNER,DIRECTION,DIRTY,DISCOVER,DISH,DIRECTION,DO,DOG,DOOR,DOUBLE,DOWN,DRAW,DREAM,DRESS,DRINK,DRIVE,DROP,DRY,DUCK,DUST,DUTY,EACH,EAR,EARLY,EARN,EARTH,EAST,EASY,EAT,EDUCATION,EFFECT,EGG,EIGHT,EITHER,ELECTRIC,ELEPHANT,ELSE,EMPTY,END,ENEMY,ENJOY,ENOUGH,ENTER,EQUAL,ENTRANCE,ESCAPE,EVEN,EVENING,EVENT,EVER,EVERY,EVERYONE,EXACT,EVERYBODY,EXAMINATION,EXAMPLE,EXCEPT,EXCITED,EXERCISE,EXPECT,EXPENSIVE,EXPLAIN,EXTREMELY,EYE,FACE,FACT,FAIL,FALL,FALSE,FAMILY,FAMOUS,FAR,FARM,FATHER,FAST,FAT,FAULT,FEAR,FEED,FEEL,FEMALE,FEVER,FEW,FIGHT,FILL,FILM,FIND,FINE,FINGER,FINISH,FIRE,FIRST,FIT,FIVE,FIX,FLAG,FLAT,FLOAT,FLOOR,FLOUR,FLOWER,FLY,FOLD,FOOD,FOOL,FOOT,FOOTBALL,FOR,FORCE,FOREIGN,FOREST,FORGET,FORGIVE,FORK,FORM,FOX,FOUR,FREE,FREEDOM,FREEZE,FRESH,FRIEND,FRIENDLY,FROM,FRONT,FRUIT,FULL,FUN,FUNNY,FURNITURE,FURTHER,FUTURE,GAME,GARDEN,GATE,GENERAL,GENTLEMAN,GET,GIFT,GIVE,GLAD,GLASS,GO,GOAT,GOD,GOLD,GOOD,GOODBYE,GRANDFATHER,GRANDMOTHER,GRASS,GRAVE,GREAT,GREEN,GREY,GROUND,GROUP,GROW,GUN,HAIR,HALF,HALL,HAMMER,HAND,HAPPEN,HAPPY,HARD,HAT,HATE,HAVE,HE,HEAD,HEALTHY,HEAR,HEAVY,HELLO,HELP,HEART,HEAVEN,HEIGHT,HELP,HEN,HER,HERE,HERS,HIDE,HIGH,HILL,HIM,HIS,HIT,HOBBY,HOLD,HOLE,HOLIDAY,HOME,HOPE,HORSE,HOSPITAL,HOT,HOTEL,HOUSE,HOW,HUNDRED,HUNGRY,HOUR,HURRY,HUSBAND,HURT,I,ICE,IDEA,IF,IMPORTANT,IN,INCREASE,INSIDE,INTO,INTRODUCE,INVENT,IRON,INVITE,IS,ISLAND,IT,ITS,JELLY,JOB,JOIN,JUICE,JUMP,JUST,KEEP,KEY,KILL,KIND,KING,KITCHEN,KNEE,KNIFE,KNOCK,KNOW,LADDER,LADY,LAMP,LAND,LARGE,LAST,LATE,LATELY,LAUGH,LAZY,LEAD,LEAF,LEARN,LEAVE,LEG,LEFT,LEND,LENGTH,LESS,LESSON,LET,LETTER,LIBRARY,LIE,LIFE,LIGHT,LIKE,LION,LIP,LIST,LISTEN,LITTLE,LIVE,LOCK,LONELY,LONG,LOOK,LOSE,LOT,LOVE,LOW,LOWER,LUCK,MACHINE,MAIN,MAKE,MALE,MAN,MANY,MAP,MARK,MARKET,MARRY,MATTER,MAY,ME,MEAL,MEAN,MEASURE,MEAT,MEDICINE,MEET,MEMBER,MENTION,METHOD,MIDDLE,MILK,MILLION,MIND,MINUTE,MISS,MISTAKE,MIX,MODEL,MODERN,MOMENT,MONEY,MONKEY,MONTH,MOON,MORE,MORNING,MOST,MOTHER,MOUNTAIN,MOUTH,MOVE,MUCH,MUSIC,MUST,MY,NAME,NARROW,NATION,NATURE,NEAR,NEARLY,NECK,NEED,NEEDLE,NEIGHBOUR,NEITHER,NET,NEVER,NEW,NEWS,NEWSPAPER,NEXT,NICE,NIGHT,NINE,NO,NOBLE,NOISE,NONE,NOR,NORTH,NOSE,NOT,NOTHING,NOTICE,NOW,NUMBER,OBEY,OBJECT,OCEAN,OF,OFF,OFFER,OFFICE,OFTEN,OIL,OLD,ON,ONE,ONLY,OPEN,OPPOSITE,OR,ORANGE,ORDER,OTHER,OUR,OUT,OUTSIDE,OVER,OWN,PAGE,PAIN,PAINT,PAIR,PAN,PAPER,PARENT,PARK,PART,PARTNER,PARTY,PASS,PAST,PATH,PAY,PEACE,PEN,PENCIL,PEOPLE,PEPPER,PER,PERFECT,PERIOD,PERSON,PETROL,PHOTOGRAPH,PIANO,PICK,PICTURE,PIECE,PIG,PIN,PINK,PLACE,PLANE,PLANT,PLASTIC,PLATE,PLAY,PLEASE,PLEASED,PLENTY,POCKET,POINT,POISON,POLICE,POLITE,POOL,POOR,POPULAR,POSITION,POSSIBLE,POTATO,POUR,POWER,PRESENT,PRESS,PRETTY,PREVENT,PRICE,PRINCE,PRISON,PRIVATE,PRIZE,PROBABLY,PROBLEM,PRODUCE,PROMISE,PROPER,PROTECT,PROVIDE,PUBLIC,PULL,PUNISH,PUPIL,PUSH,PUT,QUEEN,QUESTION,QUICK,QUIET,QUITE,RADIO,RAIN,RAINY,RAISE,REACH,READ,READY,REAL,REALLY,RECEIVE,RECORD,RED,REMEMBER,REMIND,REMOVE,RENT,REPAIR,REPEAT,REPLY,REPORT,REST,RESTAURANT,RESULT,RETURN,RICE,RICH,RIDE,RIGHT,RING,RISE,ROAD,ROB,ROCK,ROOM,ROUND,RUBBER,RUDE,RULE,RULER,RUN,RUSH,SAD,SAFE,SAIL,SALT,SAME,SAND,SAVE,SAY,SCHOOL,SCIENCE,SCISSORS,SEARCH,SEAT,SECOND,SEE,SEEM,SELLSEND,SENTENCE,SERVE,SEVEN,SEVERAL,SEX,SHADE,SHADOW,SHAKE,SHAPE,SHARE,SHARP,SHE,SHEEP,SHEET,SHELF,SHINE,SHIP,SHIRT,SHOE,SHOOT,SHOP,SHORT,SHOULD,SHOULDER,SHOUT,SHOW,SICK,SIDE,SIGNAL,SILENCE,SILLY,SILVER,SIMILAR,SIMPLE,SINGLE,SINCE,SING,SINK,SISTER,SIT,SIX,SIZE,SKILL,SKIN,SKIRT,SKY,SLEEP,SLIP,SLOW,SMOKE,SMALL,SMELL,SMILE,SMOKE,SNOW,SO,SOAP,SOCK,SOFT,SOME,SOMEONE,SOMETHING,SOMETIMES,SON,SOON,SORRY,SOUND,SOUP,SOUTH,SPACE,SPEAK,SPECIAL,SPEED,SPELL,SPEND,SPOON,SPORT,SPREAD,SPRING,SQUARE,STAMP,STAND,STAR,START,STATION,STAY,STEAL,STEAM,STEP,STILL,STOMACH,STONE,STOP,STORE,STORM,STORY,STRANGE,STREET,STRONG,STRUCTURE,STUDENT,STUDY,STUPID,SUBJECT,SUBSTANCE,SUCCESSFUL,SUCH,SUDDEN,SUGAR,SUITABLE,SUMMER,SUN,SUNNY,SUPPORT,SURE,SURPRISE,SWEET,SWIM,SWORD,TABLE,TAKE,TALK,TALL,TASTE,TAXI,TEA,TEACH,TEAM,TEAR,TELEPHONE,TELEVISION,TELL,TEN,TENNIS,TERRIBLE,TEST,THAN,THAT,THE,THEIR,THEN,THERE,THEREFORE,THESE,THICK,THIN,THING,THINK,THIRD,THIS,THOUGH,THREAT,THREE,TIDY,TIE,TITLE,TO,TODAY,TOE,TOGETHER,TOMORROW,TONIGHT,TOO,TOOL,TOOTH,TOP,TOTAL,TOUCH,TOWN,TRAIN,TRAM,TRAVEL,TREE,TROUBLE,TRUE,TRUST,TWICE,TRY,TURN,TYPE,UNCLE,UNDER,UNDERSTAND,UNIT,UNTIL,UP,USE,USEFUL,USUAL,USUALLY,VEGETABLE,VERY,VILLAGE,VOICE,VISIT,WAIT,WAKE,WALK,WANT,WARM,WASH,WASTE,WATCH,WATER,WAY,WE,WEAK,WEAR,WEATHER,WEDDING,WEEK,WEIGHT,WELCOME,WELL,WEST,WET,WHAT,WHEEL,WHEN,WHERE,WHICH,WHILE,WHITE,WHO,WHY,WIDE,WIFE,WILD,WILL,WIN,WIND,WINDOW,WINE,WINTER,WIRE,WISE,WISH,WITH,WITHOUT,WOMAN,WONDER,WORD,WORK,WORLD,WORRY,WORRY,WORST,WRITE,WRONG,YEAR,YES,YESTERDAY,YET,YOU,YOUNG,YOUR,ZERO"};

#define N_BUFFERS 5
char DSY_SDRAM_BSS buffers[N_BUFFERS][22050*10]; // Enough

uint8_t activeBufferIdx = 0, nextBufferIdx = 0, lastBufferIdx = 0;

float bufPos = 0;
float lastBufPos = (22050*10);
float xfade = FADE_LEN;

uint32_t bufferLengths[N_BUFFERS] = {
    0,0,0,0,0
};

bool isPlaying = false;

void runSam(char *pText, uint8_t bufIdx) {
	unsigned char input[256];
	memset(input, 0, 256);
    int i = 0;
    bool silence = true;
    while(pText[i] != 0) {
        if (pText[i] != ' ') {
            silence = false;
        }
        input[i] = pText[i];
        i++;
    }
    if (!silence) {
        TextToPhonemes(input);
        SetInput(input);

        (void)SAMMain((char*) &buffers[bufIdx][0]);
        bufferLengths[bufIdx] = GetBufferLength() / 50;

        i = 0;
        while (i < FADE_LEN) {
            float a = (float)i / FADE_LEN;
            float s =  ((float)buffers[bufIdx][i]);
            //buffers[bufIdx][i] = (s * a);

            s = ((float)buffers[bufIdx][bufferLengths[bufIdx] - i]);
            buffers[bufIdx][bufferLengths[bufIdx] - i] = (s * a);
            i++;
        }

        i = 0;
        while (i < 96) {
            buffers[bufIdx][bufferLengths[bufIdx] + i] = 0;
            i++;
        }
    } else {
        bufferLengths[bufIdx] = FADE_LEN;
        memset(&buffers[bufIdx][0], 0, FADE_LEN + 96);
    }
}

enum menuItemType_e {
    MI_TRIGGER,
    MI_TEXTINPUT,
    MI_RANDOM,
    MI_PROFILE,
    MI_RESET,
    MI_LAST
};

const char triggerLabels[3][2] = {
    {"-"}, {"+"}, {"?"}
};

enum triggerType_e {
    TT_NONE,
    TT_ALWAYS,
    TT_RANDOM,
    TT_LAST    
};

enum profiles_e {
    PR_ELF,
    PR_ROBOT,
    PR_GUY,
    PR_LADY,
    PR_ET,
    PR_SAM,
    PR_LAST
};

#define N_PROFILES 6
const char profileLabels[N_PROFILES][7] = {
    "Sam",
    "Elf",
    "Robot",
    "Guy",
    "Lady",
    "ET",  
};

struct profile {
    uint8_t speed, pitch, throat, mouth;
};

profile profiles[N_PROFILES] = {
    {72,64,128,128},
    {72,64,110,160},
    {92,60,190,190},
    {82,72,110,105},
    {82,32,145,145},
    {100,64,150,200}
};

#define TEXT_LEN 29
#define TEXT_SCREEN_LEN 12

char paramText[N_BUFFERS][TEXT_LEN] = {
    {"WORK                        "}, 
    {"REMEMBER                    "}, 
    {"OFFICE                      "}, 
    {"TRANSFER                    "}, 
    {"COMMUNICATION               "}, 
};

uint8_t paramNum[N_BUFFERS] = {
    TT_ALWAYS, TT_ALWAYS, TT_ALWAYS, TT_ALWAYS, TT_ALWAYS,
};

uint8_t paramProfile = 0;

struct menuItem_t {
    uint8_t *pParameter;
    uint8_t itemType;
    uint8_t idx;
};


void runSamAll() {

	SetPitch(profiles[paramProfile].pitch);
    SetSpeed(profiles[paramProfile].speed);
    SetMouth(profiles[paramProfile].mouth);
    SetThroat(profiles[paramProfile].throat);

    uint8_t i = 0;
    while (i < N_BUFFERS) {
        runSam((char*)&paramText[i][0], i);   
        i++;
    }
}

#define N_MENU_ITEMS 17
struct uiControl {
    menuItem_t menu[N_MENU_ITEMS] = {
        {&paramNum[0], menuItemType_e::MI_TRIGGER, 0},
        {(uint8_t*)&paramText[0][0], menuItemType_e::MI_TEXTINPUT, 0},
        {(uint8_t*)&paramText[0][0], menuItemType_e::MI_RANDOM,0},

        {&paramNum[1], menuItemType_e::MI_TRIGGER, 1},
        {(uint8_t*)&paramText[1][0], menuItemType_e::MI_TEXTINPUT, 1},
        {(uint8_t*)&paramText[1][0], menuItemType_e::MI_RANDOM,1},

        {&paramNum[2], menuItemType_e::MI_TRIGGER, 2},
        {(uint8_t*)&paramText[2][0], menuItemType_e::MI_TEXTINPUT, 2},
        {(uint8_t*)&paramText[2][0], menuItemType_e::MI_RANDOM,2},

        {&paramNum[3], menuItemType_e::MI_TRIGGER, 3},
        {(uint8_t*)&paramText[3][0], menuItemType_e::MI_TEXTINPUT, 3},
        {(uint8_t*)&paramText[3][0], menuItemType_e::MI_RANDOM, 3},

        {&paramNum[4], menuItemType_e::MI_TRIGGER, 4},
        {(uint8_t*)&paramText[4][0], menuItemType_e::MI_TEXTINPUT, 4},
        {(uint8_t*)&paramText[4][0], menuItemType_e::MI_RANDOM, 4},

        {&paramProfile, menuItemType_e::MI_PROFILE, 0},
        {nullptr, menuItemType_e::MI_RESET,0}
        
    };

    uint32_t screen_update_last_;
    uint32_t screen_update_period_;

    uint8_t menuIdx;
    int8_t menuSubIdx;
    uint8_t editMode;
    bool blink;

    bool itemChanged;

    void init() {
        menuIdx = 0;  
        menuSubIdx = 0;
        editMode = 0;  
    
        screen_update_period_ = 170;
        screen_update_last_ = patch.seed.system.GetNow();

        itemChanged = false;
    }

    void update() {
      
        patch.ProcessDigitalControls();
        int32_t increment = patch.encoder.Increment();

        if (patch.encoder.RisingEdge()) {
            editMode++;
            if (editMode == 1) {
                if ((int8_t)menu[menuIdx].itemType == MI_RESET) {
                   editMode = 0;
                } else if ((int8_t)menu[menuIdx].itemType == MI_RANDOM) {
                    uint16_t p = (nextRnd() % (N_WORD_CHARS - 1)) + 1;
                    while (words[p] != ',') {
                        p--;
                    }
                    p++;
                    uint16_t d = 0;
                    while (d < (TEXT_LEN - 1)) {
                        if (words[p] != ',' && words[p] != 0) {
                            menu[menuIdx].pParameter[d] = words[p++];
                        } else {
                            menu[menuIdx].pParameter[d] = 32;
                        }
                        d++;
                    }
                    menu[menuIdx].pParameter[d] = 0;
                    runSam((char*) &menu[menuIdx].pParameter[0], menu[menuIdx].idx);
                    editMode = 0;
                }
                menuSubIdx = 0;
                itemChanged = false;
            } else if ((int8_t)menu[menuIdx].itemType == MI_TEXTINPUT) {
                if (menuSubIdx == (TEXT_LEN - 1)) {
                    if (itemChanged) {
                        runSam((char*) &menu[menuIdx].pParameter[0], menu[menuIdx].idx);
                    }
                    itemChanged = false;
                    editMode = 0;
                } else if (editMode > 2) {
                    editMode = 1;
                }
            } else {
                if ((int8_t)menu[menuIdx].itemType == MI_PROFILE && itemChanged) {
                    runSamAll();
                }
                editMode = 0;
            }
        }
        
        patch.encoder.Debounce();

        if (!editMode) {
            menuIdx += increment;
            if (menuIdx >= N_MENU_ITEMS) {
                menuIdx = 0;    
            }
            blink = false;
        } else {
             if(patch.seed.system.GetNow() - screen_update_last_ > screen_update_period_) {
                blink ^= true;
                screen_update_last_ = patch.seed.system.GetNow();
             }
        }
        uint8_t y = 0;
        patch.display.SetCursor(0, y);
        uint8_t i = 0;
        while (i < N_MENU_ITEMS) {
            bool fill = (menuIdx != i);
            bool selected = false;
            if (!fill && editMode) {
                selected = true;
            }
           
            if (!fill) {
                fill ^= blink;
            }
            patch.display.WriteChar('|', Font_7x10, true);
            uint8_t j = 0;
            uint8_t offset = 0;
            int8_t p = 0;

            switch((int8_t)menu[i].itemType) {
                case MI_TRIGGER:
                    p = *(menu[i].pParameter);
                    if (selected && increment != 0) {
                        p += increment;
                        if (p >= TT_LAST) {
                            p = TT_LAST - 1;    
                        } else if (p < 0) {
                            p = 0;
                        }
                        *menu[i].pParameter = p;
                    }
                 
                    patch.display.WriteString(&triggerLabels[p][0], Font_7x10, fill);
                    break;
                case MI_TEXTINPUT:
                    if (selected) {
                        if (editMode == 1) {
                            menuSubIdx += increment;
                            if (menuSubIdx < 0) {
                                menuSubIdx = 0;       
                            } else if (menuSubIdx >= TEXT_LEN - 1) {
                                menuSubIdx = (TEXT_LEN - 1);   
                            }
                        } else {
                            int8_t v = menu[i].pParameter[menuSubIdx];
                            v += increment;
                            if (increment < 0) {
                                if (v < 'A') {
                                    v = ' ';
                                }
                            } else if (increment > 0) {
                                if (v > 'Z') { 
                                    v = 'Z';
                                } else if (v > ' ' && v < 'A') {
                                    v = 'A';
                                }
                            }
                            if (menu[i].pParameter[menuSubIdx] != v) {
                                menu[i].pParameter[menuSubIdx] = v;
                                itemChanged = true;
                            }
                        }

                        offset = (TEXT_SCREEN_LEN * 0.5) * (int8_t)(menuSubIdx / (TEXT_SCREEN_LEN * 0.5));
                        if (offset > (TEXT_LEN - TEXT_SCREEN_LEN)) {
                            offset = (TEXT_LEN - TEXT_SCREEN_LEN) - 1;
                        }

                        while (j < TEXT_SCREEN_LEN && (j + offset) < (TEXT_LEN - 1)) {
                            fill = (menuSubIdx != j + offset);
                            if (!fill && editMode == 2) {
                                fill ^= blink;
                            }
                            patch.display.WriteChar(menu[i].pParameter[j + offset], Font_7x10, fill);
                            j++;
                        }
                        
                        patch.display.WriteChar('|', Font_7x10, true);
                        if (menuSubIdx == (TEXT_LEN - 1)) {
                            fill = false;    
                        } else {
                            fill = true;
                        }
                        patch.display.WriteChar('s', Font_7x10, fill);

                    } else {
                        while (j < TEXT_SCREEN_LEN) {
                            patch.display.WriteChar((menu[i].pParameter[j]), Font_7x10, fill);
                            j++;
                        }
                    }

                    break;
                
                case MI_RANDOM:
                    patch.display.WriteString("r", Font_7x10, fill);
                    break;

                case MI_PROFILE:
                    p = *(menu[i].pParameter);
                    if (selected && increment != 0) {
                        p += increment;
                        if (p >= PR_LAST) {
                            p = PR_LAST - 1;    
                        } else if (p < 0) {
                            p = 0;
                        }   
                        if (p != *menu[i].pParameter) {
                            *menu[i].pParameter = p;
                            itemChanged = true;
                        }
                    }
                    patch.display.WriteString(&profileLabels[p][0], Font_7x10, fill);
                    break;
                
                case MI_RESET:
                    patch.display.WriteChar('R', Font_7x10, fill);
                    break;
            }
            if ((int8_t)menu[i].itemType == MI_RANDOM) {
               
                if (activeBufferIdx == menu[i].idx) {
                    patch.display.WriteChar('*', Font_7x10, true);
                } else {
                    patch.display.WriteChar('|', Font_7x10, true);
                }

                y+=10;
                patch.display.SetCursor(0, y);
            }
            i++;
        }
        patch.display.WriteString("|", Font_7x10, true);
    }
};

uiControl ui;

static void AudioCallback(AudioHandle::InputBuffer  in,
                          AudioHandle::OutputBuffer out,
                          size_t                    size)
{
    patch.ProcessAnalogControls();

    if(patch.gate_input[0].Trig()) {
        
        if (paramNum[nextBufferIdx] != TT_NONE) {
            lastBufferIdx = activeBufferIdx;
            lastBufPos = bufPos;

            activeBufferIdx = nextBufferIdx;
            bufPos = 0;
            xfade = bufPos;
            isPlaying = true;
        }

        if (paramNum[activeBufferIdx] == TT_RANDOM) {
            nextBufferIdx = (nextRnd() % N_BUFFERS);
        }

        uint8_t c = N_BUFFERS;
        while (c) {
            c--;
            nextBufferIdx++;
            if (nextBufferIdx >= N_BUFFERS) {
                nextBufferIdx = 0;
            }
            if (paramNum[nextBufferIdx] != TT_NONE) {
                break;
            }
           
        }
    } else if (isPlaying && patch.gate_input[1].Trig() && bufPos >= FADE_LEN) {
        lastBufferIdx = activeBufferIdx;
        lastBufPos = bufPos;

        float rep = repeat.Process();
        if (rep > bufPos) {
            bufPos = 0;
        } else {
            bufPos -= rep;
        }
        xfade = 0;
    }

    char *pLastBuffer = (char*) &buffers[lastBufferIdx][0];
    uint32_t lastBufferLength = bufferLengths[lastBufferIdx];

    char *pBuffer = (char*) &buffers[activeBufferIdx][0];
    uint32_t bufferLength = bufferLengths[activeBufferIdx];
 
    float stp = playbackSpeed.Process();
    float att = attenuator.Process();

    filter.SetFreq(cutoffLow.Process());

    float *pOut = &out[0][0];

    if (!isPlaying) {
        for(size_t i = 0; i < size; i++) {
            *pOut++ = 0;
        }
    } else {
        for(size_t i = 0; i < size; i++) {
            uint32_t pos = (uint32_t) bufPos;
            float fr = bufPos - pos;
           
            float s1 = ((float)pBuffer[pos++]) / 128.f;
            float s2 = ((float)pBuffer[pos]) / 128.f;
#ifdef ADD_NOISE
            float s = (s1 * (1.0 - fr) + s2 * fr + noise.Process());
#else
            float s = (s1 * (1.0 - fr) + s2 * fr);
#endif

            if (xfade < FADE_LEN) {
                float d = (xfade / FADE_LEN);
                s = s * d;
                if (lastBufPos < lastBufferLength) {
                    pos = (uint32_t) lastBufPos;
                    fr = lastBufPos - pos;

                    s1 = ((float)pLastBuffer[pos++]) / 128.f;
                    s2 = ((float)pLastBuffer[pos]) / 128.f;
                    d = (1.f - d);
#ifdef ADD_NOISE
                    s = s + ((s1 * (1.0 - fr) + s2 * fr + noise.Process()) * d);
#else
                    s = s + ((s1 * (1.0 - fr) + s2 * fr) * d);
#endif
                    lastBufPos += stp;
                }
                xfade += stp;
            }

            filter.Process(s);
            s = filter.Low();
        
            filterPost.Process(s);
            *pOut++ = filterPost.High() * att;

            bufPos +=stp;
        }
    }

    if (bufPos >= bufferLength) {
        isPlaying = false;
    }

    dsy_gpio_write(&patch.gate_output, isPlaying);
  
}

int main(void)
{
    patch.Init(); // Initialize hardware (daisy seed, and patch)
    float samplerate = patch.AudioSampleRate();

    filter.Init(samplerate);
    filter.SetRes(0.f);

    filterPost.Init(samplerate);
    filterPost.SetRes(0.f);
    filterPost.SetFreq(200.f);

    playbackSpeed.Init(patch.controls[0], 0.25, 0.75, Parameter::LINEAR);
    attenuator.Init(patch.controls[1], 0, 1, Parameter::LINEAR);
    cutoffLow.Init(patch.controls[2], 200, 10000, Parameter::LOGARITHMIC);
    repeat.Init(patch.controls[3], FADE_LEN, FADE_LEN * 4.f, Parameter::LINEAR);

    noise.Init();
    noise.SetAmp(0.025);

    patch.ProcessAnalogControls();
    srnd[0] = playbackSpeed.Process() * (float)0xffffffff;
    srnd[1] = attenuator.Process() * (float)0xffffffff;
    srnd[2] = cutoffLow.Process() * (float)0xffffffff;
    srnd[3] = repeat.Process() * (float)0xffffffff;

    ui.init();

    runSamAll();

    patch.display.WriteString("LOAD\"*\",8,1", Font_7x10, true);
    patch.display.Update();
    patch.DelayMs(1000);

    patch.StartAdc();
    patch.StartAudio(AudioCallback);
    while(1)
    {
        patch.display.Fill(false);
        ui.update();
        patch.display.Update();
    }
}
