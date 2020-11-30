#include <iostream>
#include <fstream>
#include <time.h>

// class declaration for the skiplist node
class skip_list_node 
{
    public:
        // a dynamic array of pointer to skiplist nodes
        struct skip_list_node** forward;
        // a key value between [9999 999999]
        int key;
        // a string associated with a given key between [4, 20] uppercase chars
        std::string val;
        // constructor for skiplist node (Key, level, val)
        skip_list_node(int, int, std::string);
       
};

//implementation of constructor for skiplist node
skip_list_node::skip_list_node(int Key, int level, std::string value)
{
    this->key = Key;
    this->val = value;
    forward = new skip_list_node* [level + 1];
    // Fills a block of memory with space needed for forward (allows for dynamic memory allocation)
    memset(forward, 0, sizeof(skip_list_node*)*(level + 1));
};

// class declartation of skiplist
class skip_list 
{
    //pointer to a dummy skip list node with initial height of maxHeight
    skip_list_node* header;
    // current number of levels in the skiplist
    int level;
    int maxLevel;
    // probability, set for the whole skiplist
    float p;

    public:
    
        skip_list();
        int randomLevel();
        skip_list_node* newNode(int, int, std::string);
        bool insert(int, std::string);
        bool remove(int);
        bool isPresent(int);
        std::string find(int);
        int membership();
        void listAll();
        void debugList();
        int getlevel(int);
    


};

//skipList constructor
skip_list::skip_list()    
{
    p = .25;
    maxLevel = 5;
    level = 1;
   
    //dummy header node with height of max level
    header = new skip_list_node(100000, maxLevel, "head");
}

// sets the level that a new node will have (randomly generated)
int skip_list::randomLevel()
{
    int newLevel = 1;
  
    while ((float)std::rand() / RAND_MAX < p && newLevel < maxLevel)
    { 
       newLevel = newLevel + 1;
    }
    return std::min(newLevel, maxLevel);
};

//makes a new node
skip_list_node* skip_list::newNode(int Key, int level, std::string s)
{
    skip_list_node* node = new skip_list_node(Key, level, s);
    return node;
}

//inserts key into list if not already present
bool skip_list::insert(int Key, std::string s)
{
    bool inserted = false;
    if (isPresent(Key))
    {
        return inserted;
    }

    //x is the current node
    skip_list_node* x = header;
    //create update array with [maxLevel]
    skip_list_node* update[5 +1];
    //dedicate memory for the update array
    memset(update, 0, sizeof(skip_list_node*) * (maxLevel + 1));

    //traverse through the skiplist, starting at the highest level to the lowest
    for (int i = level; i >= 1; i--)
    {
        while (x->forward[i] != nullptr && x->forward[i]->key < Key)
        {
            x = x->forward[i];
        }
        update[i] = x;
    }
    
    x = x->forward[1];
 
    if (x == nullptr || x->key != Key)
    {
        // Generate a random level for node 
        int newLevel= randomLevel();

       // if our new level is > then the highest level in the list,
       //initialize update for further use
        if (newLevel > level)
        {
            for (int i = level + 1; i < newLevel + 1; ++i) 
            {
                update[i] = header;
            }
            level = newLevel;
        }

        // create new node to be inserted
        x = newNode(Key, newLevel, s);

        // insert node by rearranging pointers 
        for (int i = 1; i <= newLevel; i++)
        {
            x->forward[i] = update[i]->forward[i];
            update[i]->forward[i] = x;
        }
        inserted = true;
       
    }
   
    return inserted;
}

//removes key if key is present
bool skip_list::remove(int Key)
{
    bool removed = false;
    if (!isPresent(Key))
    {
        return removed;
    }

    //x is the current node
    skip_list_node* x = header;
    //create update array with [maxLevel + 1]
    skip_list_node* update[5 + 1];
    //set aside memory for the update array
    memset(update, 0, sizeof(skip_list_node*) * (maxLevel + 1));

    //traverse the skiplist
    for (int i = level; i >= 1; i--)
    {
        while (x->forward[i] != nullptr && x->forward[i]->key < Key)
            x = x->forward[i];

        update[i] = x;
    }


    x = x->forward[1];


    if (x != nullptr && x->key == Key)
    {
        for (int i = 1; i <= level; i++)
        {
            if (update[i]->forward[i] != x)
                break;
            update[i]->forward[i] = x->forward[i];

        }

        //delete the node
        delete x;
        //adjusts levels accordingly
        while (level > 1 && header->forward[level] == header)
        {
            level = level - 1;
        }

        removed = true;
    }

    return removed;

};

//searches skiplist to see is Key exists
bool skip_list::isPresent(int Key)
{
    bool found = false;
    // node x is the current node
    skip_list_node* x = header;
    for (int i = level; i >= 1; i--)
    {
        while (x->forward[i] != nullptr && x->forward[i]->key < Key)
            x = x->forward[i];
    }

    x = x->forward[1];

    // if the current node's key is equal to the desired key, it is present
    if (x != nullptr && x->key == Key)   
        found = true;
    
    return found;
};

//searches the skiplist and returns the value of the key if present
std::string skip_list::find(int Key)
{
    std::string found = "";
    if (!isPresent(Key))
    {
        return found;
    }
    else
    {
        //traverse list to find node with correct key
        skip_list_node* x = header;
        for (int i = level; i >= 1; i--)
        {
            while (x->forward[i] != nullptr && x->forward[i]->key < Key)
               x =x->forward[i];
        }
        x = x->forward[1];
        // we have found the key
        if (x != nullptr && x->key == Key)
            found = x->val;
    }
   
    return found;
    
}

//prints out the number of members (key, string pairs) in the list
int skip_list::membership()
{
    // x is the current node
    skip_list_node* x = header;
    int nodeCount = 0;

    //traverses the array
    while (x->forward[1] != nullptr)
    {
        nodeCount++;
        x = x->forward[1];
    }
    
    return nodeCount;
}

// prints all elements in increasing key order, with their associated string values
void skip_list::listAll()
{
    //x is the current node
    skip_list_node* x = header;

    while (x->forward[1] != nullptr)
    {
        std::cout << x->forward[1]->key << " " << x->forward[1]->val  << "\n";
        x = x->forward[1];
    }

}

// prints all elements in increasing key order, with their associated levels
void skip_list::debugList()
{
    // x is the current node
   skip_list_node* x = header;

    while (x->forward[1] != nullptr)
    {
        std::cout << x->forward[1]->key << " " << getlevel(x->forward[1]->key) << "\n";
        x = x->forward[1];
    }

}

//gets the level of a given node
int skip_list::getlevel(int Key)
{
    //x is the current node
    skip_list_node* x = header;

    //the node level that will be returned
    int returnLevel = 1;
    
    //traverse through the skiplist
    for (int i = level; i >= 1; i--)
    {
        while (x->forward[i] != nullptr && x->forward[i]->key < Key)
        {
            x = x->forward[i];
           
        }
        // if we have found the correct key, then i is the associated level
        if (x->forward[i] != nullptr && x->forward[i]->key == Key)
        {
            returnLevel = i;
            break;
        }

    }

    return returnLevel;

}

//main function with menu to accept commands from input file
int main()
{
    //initialize random seed
    srand((unsigned)time(0));
    // file to read
    std::ifstream myfile;
    myfile.open("SkipListData.txt");
    
    //variables necessary for commands
    skip_list lst;
    std::string val;
    char command;
    int key;
    std::string find;
 
    while (myfile >> command)
    {
        switch (command)
        {
        case 'A':
            myfile >> key >> val;
            if (lst.insert(key, val))
            {
                std::cout << val << " inserted" << "\n";
            }
            else 
            {
                std::cout << "Key " << key << " already exists" << "\n";
            }
            break;
        case 'D':
            myfile >> key;

            if (lst.remove(key))
            {
                std::cout << "Key " << key << " deleted" << "\n";
            }
            else
            {
               std::cout << "Key " << key << " not found" << "\n";
            }
            break;
        case 'S':
            myfile >> key;
            find = lst.find(key);
            if (find == "")
            {
                std::cout << "Key " << key << " not found" << "\n";
            }
            else
            {
                std::cout << "Key " << key << " found, value " << find << "\n";
            }
            break;
        case 'M':
            std::cout << "Membership is "<< lst.membership() << "\n";
            break;
        case 'L':
            lst.listAll();
            break;
        case 'T':
            lst.debugList();
            break;
        case 'E':
            exit;

        }
        
    }
   
    myfile.close();
    return 0;
}

