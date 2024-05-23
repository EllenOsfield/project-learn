#include <iostream>

using namespace std;

void printGrid(int &userRows, int &userColumns){
  cout<<endl;
  cout<<" ";
  int i=1,j;
  for(j = 0; j <= 4*userColumns; j++){
    if(j%4==2)
        cout<<i++;
    else cout<<" ";
  }
  cout<<endl;
  for(i = 0; i <= 2*userRows; i++){
    if(i%2!=0)
      cout<<(char)(i/2 +'A');
    for(j = 0; j <= 2*userColumns; j++){
      if(i%2==0)
      {
        if(j==0)
            cout<<" ";
        if(j%2==0)
            cout<<" ";
        else cout<<"---";
      }
      else{
        if(j%2==0)
            cout<<"|";
        else cout<<"   ";
      }
    }
    if(i%2!=0)
      cout<<(char)(i/2 +'A');
    cout<<endl;
  }
  cout<<" ";
  for(j = 0, i = 1; j <= 4*userColumns; j++){
    if(j%4==2)
        cout<<i++;
    else cout<<" ";
  }
  cout<<endl;
}

int main() {
  int userRows, userColumns;
  cout << "Enter the number of rows -> ";
  cin >> userRows;
  cout << "Enter the number of columns -> ";
  cin >> userColumns;
  printGrid(userRows, userColumns);
  return 0;
}
