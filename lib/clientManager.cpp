/**
 * @file clientManager.cpp
 * @author 11796444 - Giovanni Shibaki Camargo 
 * @author 11796472 - Lucas Keiti Anbo Mihara
 * @author 11795589 - Vitor Caetano Brustolin
 * @brief 
 * @version 0.1
 * @date 2022-06-04
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <iostream>
#include <string>

using namespace std;

class clientManager
{
private:
    string _username;
    
public:
    clientManager()
    {

    }

    string getUsername() 
    {
        return _username;
    }

    void setUsername(string username)
    {
        _username = username;
    }


};


