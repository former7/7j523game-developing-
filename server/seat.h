#include <inttypes.h>
#include <map>
class seat {

public:
    seat();

    seat(int8_t n);

    void newRound();

    int8_t getNumber() { return seatNumber; };

    int getRemainingChips() { return remainingChips; };

    void wins(int n);

    void setFD(int n) { fd = n; };

    int getFD() { return fd; };

    void setName(unsigned char *_name) { name = _name; };

    unsigned char* getName() { return name; };
    vector<int>& getHandCards(){return handCards;};
    int getHandNum(){return handCards.size();}
    ~seat();

private:
    unsigned char *name;

    int8_t seatNumber;

    int fd;

    int remainingChips;
    
    map<int,int> handCards;
    
    int score;  

};
