#include <cassert> // for assert()
#include <initializer_list> // for std::initializer_list
#include <iostream>
#include <array>
#include <ctime>
#include <random>
#include <cstdlib>
#include <experimental/random>

// Classes

class Potion{
public:
    enum class Size{
        small,
        medium,
        large,
        max_sizes
    };

    enum class Type{
        health,
        strength,
        poison,
        weakness,
        max_types
    };

private:
    Type m_type{};
    Size m_size{};
    int m_dHealth{};
    int m_dDamage{};

public:
    Potion(Type type, Size size, int dHealth, int dDamage) :  m_type{ type }, m_size{ size }, m_dHealth{ dHealth }, m_dDamage{ dDamage }{};

    int getDHealth() const { return m_dHealth; }
    int getDDamage() const { return m_dDamage; }

    static const Potion& getDefaultPotion(Type t, Size s) {
        static std::array<Potion, static_cast<int>(Type::max_types) * static_cast<int>(Size::max_sizes)> potionData{
            {       // type         //size       //h    //s
                {Type::health,      Size::small,    2,  0},
                {Type::health,      Size::medium,   3,  0},
                {Type::health,      Size::large,    5,  0},
                {Type::strength,    Size::small,    0,  1},
                {Type::strength,    Size::medium,   0,  2},
                {Type::strength,    Size::large,    0,  3},
                {Type::poison,      Size::small,    -1, 0},
                {Type::poison,      Size::medium,   -2, 0},
                {Type::poison,      Size::large,    -3, 0},
                {Type::weakness,    Size::small,    0,  -1},
                {Type::weakness,    Size::medium,   0,  -1},
                {Type::weakness,    Size::large,    0,  -2}
            }
        };

        int index{ static_cast<int>(t)*static_cast<int>(Size::max_sizes) + static_cast<int>(s) };
        return potionData.at(index);
    }

    static Potion getRandomPotion(){
        Type t{ static_cast<Type>( std::experimental::randint(0,static_cast<int>(Type::max_types)-1) ) };
        Size s{ static_cast<Size>( std::experimental::randint(0,static_cast<int>(Size::max_sizes)-1)) };
        return getDefaultPotion(t, s);
    }

    friend std::ostream& operator<<(std::ostream& out, Potion& p);
};

class Creature{
protected:
    std::string m_name{};
    char m_symbol{};
    int m_health{};
    int m_damage{};
    int m_gold{};
public:
    Creature(std::string_view name, char symbol, int health, int dpa, int gold) 
        : m_name{ name }, 
        m_symbol{ symbol },
        m_health{ health }, 
        m_damage{ dpa }, 
        m_gold{ gold }{
    }

    void addGold(int gold){
        m_gold += gold;
    }

    void reduceHealth(int damage){
        m_health -= damage;
    }

    bool isDead(){
        return m_health <= 0;
    }

    const std::string& getName() const { return m_name; }
    int getHealth() const { return m_health; }
    int getGold() const { return m_gold; } 
    int getDamage() const { return m_damage; }
    char getSymbol() const { return m_symbol; }

    void attack(Creature& c){
        std::cout << m_name << " hit " << c.m_name << " for " << m_damage << " damage.\n";
        c.reduceHealth(m_damage);
    }

    void drink(const Potion& p){
        m_health += p.getDHealth();
        m_damage += p.getDDamage();
    }
};

class Player : public Creature{
private:
    int m_level{ 1 };
public:
    Player(std::string_view name) : 
        Creature(name, '@', 10, 1, 0)
    {};

    int getLevel() const { return m_level; }
    bool hasWon() const { return m_level>=20; }

    void levelUp(){
        ++m_level;
        ++m_damage;
    }
};

class Monster : public Creature{
public:
    enum class Type{
        dragon,
        orc,
        slime,
        max_types
    };

private:
    static const Creature& getDefaultCreature(Type type){
        static const std::array<Creature, static_cast<std::size_t>(Type::max_types)> monsterData{       //This is a lookup table. Now when we want monster info we can get it from here. For ex: int dragon_gold = getDefaultCreature(Type::dragon).getGold();
            {
                {"dragon", 'D', 20, 4, 100},
                {"orc", 'o', 4, 2, 25},
                {"slime", 's', 1, 1, 10 }
            }
        };

        return monsterData.at(static_cast<std::size_t>(type));
    }

public:
    Monster(Type type) 
        : Creature(getDefaultCreature(type))
    {}

    static Monster getRandomMonster(){
        int mi{ std::experimental::randint(0, static_cast<int>(Type::max_types)-1) };
        return {static_cast<Type>(mi)};
    }
};

// Friends

static std::string toString(Potion::Size s){
    switch(s){
    case Potion::Size::small: return "small";
    case Potion::Size::medium: return "medium";
    case Potion::Size::large: return "large";
    default: assert(false && "Not handled");
    }
}

static std::string toString(Potion::Type t){
    switch (t)
    {
    case Potion::Type::health: return "health";
    case Potion::Type::strength: return "strength";
    case Potion::Type::poison: return "poison";
    case Potion::Type::weakness: return "weakness";
    default: assert(false && "Not handled"); 
    }
}

std::ostream& operator<<(std::ostream& out, Potion& p){
    out << toString(p.m_size) << ' ' << toString(p.m_type) << " potion";
    return out;
}

// Functions

Player getPlayer(){
    std::cout << "Enter a name for your character: ";
    std::string name;
    std::cin >> name;
    std::cout << "Welcome, " << name << ".\n";
    return {name};
}

// true for run, false for fight
bool getRunOrFight(){

    while(true){
        std::cout << "(R)un or (F)ight: ";
        char choice;
        std::cin >> choice;
        if(choice == 'R' || choice == 'r'){
            return true;
        } 
        if(choice == 'F' || choice == 'f'){
            return false;
        }
    }
}

bool getDrink(Player& p){

    while(true){
        std::cout << p.getName() << " found a mysterious potions! Drink it? [y/n]: ";
        char choice;
        std::cin >> choice;
        if(choice == 'y'){
            return true;
        }
        if(choice == 'n'){
            return false;
        }
    }
}

// true if the encounter should end
bool tryRunning(Player& p, Monster& m){
    if(std::experimental::randint(0,1)){
        std::cout << p.getName() << " successfully fled.\n";
        return true;
    }

    std::cout << p.getName() << " failed to flee!\n";
    m.attack(p);
    return p.isDead();
}

void findPotion(Player& p){

    if(std::experimental::randint(0,9)<3 && getDrink(p)){
        Potion potion{ Potion::getRandomPotion() };
        std::cout << p.getName() << " drank a " << potion << ".\n";
        p.drink(potion);
    }
}

void handleMonsterDeath(Player& p, Monster& m){
    std::cout << p.getName() << " killed the " << m.getName() << ".\n";
    p.levelUp();
    std::cout << p.getName() << " is now level " << p.getLevel() << ".\n";
    p.addGold(m.getGold());
    std::cout << p.getName() << " found " << m.getGold() << " gold.\n";
    findPotion(p);
}

// true if the encounter should end
bool doCombatRound(Player& p, Monster& m){
    p.attack(m);
    if(m.isDead()){
        handleMonsterDeath(p, m);
        return true;
    }
    m.attack(p);
    return p.isDead();
}

void handleEncounter(Player& p){

    bool ended{ false };
    Monster m{ Monster::getRandomMonster() };
    std::cout << p.getName() << " has encountered a " << m.getName() << " (" << m.getSymbol() << ").\n";
    while(!ended){
        bool run{ getRunOrFight()};
        if(run){
            ended = tryRunning(p, m);
        } else {
            ended = doCombatRound(p, m);
        }
    }
    std::cout << p.getName() << " has " << p.getHealth() << " hitpoints.\n";
}

bool playGame(Player& p){

    while(true){        
        handleEncounter(p);
        if(p.isDead()){
            return false;
        }
        if(p.hasWon()){
            return true;
        }
    }
}


int main()
{
	Player p{getPlayer()};
    if(playGame(p)){
        std::cout << "Congrats! " << p.getName() << " got away with " << p.getGold() << " gold.\n";
    } else {
        std::cout << p.getName() << " died at level " << p.getLevel() << " with " << p.getGold() << " gold.\n";
        if(p.getGold()>0){
            std::cout << "Too bad you can't take it with you!\n";
        }
    }

	return 0;
}