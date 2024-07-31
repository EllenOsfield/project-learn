#include <iostream>

void doPrint()
{
    std::cout<<"In doPrint()\n";
}

void doublenumber()
{
    std::cout<<"Enter an integer: ";
    int num{};
    std::cin>>num;
    std::cout <<num << " doubled is: " << num * 2 << "\n";
}

int returnFive()
{
    return 5;
}

int getvalformuser()
{
    std::cout<<"Enter a number: ";
    int input{};
    std::cin>>input;

    return input;
}

void printval(int x)
{
    std::cout << x << "\n";
}

int add(int x, int y)
{
    return x+y;
}

int main()
{
    std::cout<<"Starting main()\n";
    doPrint();
    doPrint();
    doublenumber();
    std::cout<< returnFive()<<"\n"; //prints 5
    std::cout<<returnFive() + 4 <<"\n"; //prints 7
    returnFive(); //returns 5 but doesnt do anything with it
    int num{getvalformuser()};
    std::cout<<num<<" triple is: "<<num*3<<"\n";
    int x{getvalformuser()};
    int y{getvalformuser()};
    std::cout<<x<< " + "<<y<<" = "<<x+y<<"\n";
    printval(add(5,6));
    std::cout<<"Ending main()\n";


    return 0;
}
