    #include <fstream>
    #include <cstdio>
    #include <string>
    #include <bits/stdc++.h>
    #include <iostream>
    #include <random>
using namespace std;

int main()
{
    int n=0, r, t=0;
    string format=".txt";
    cout << "Insert how many names you want: " << endl;
    cin>>n;
    string A[n], B[n], final_file_name;
    for (int i=0;i<n;i++)
    {
        cout << "Insert the "<<i+1;
        switch( (i+1)%10)
        {
        case 1:
            if ((i+1)%11!=0)
                cout<<"st";
            else
                cout<<"th";
            break;
        case 2:
            if ((i+1)%12!=0)
                cout<<"nd";
            else
                cout<<"th";
            break;
        case 3:
            if ((i+1)%13!=0)
                cout<<"rd";
            else
                cout<<"th";
            break;
        default:
            cout<<"th";
        }
        cout<<" name: " << endl;
        cin>>A[i];
        B[i]=A[i];
    }



    //problem with names check
        for (int i=0;i<n-1;i++)
            for (int j=i+1;j<n;j++)
                if (A[i]==A[j])
                    {
                        n=0;
                        t=-1;
                    }



    if (n>2)
     {
        ofstream file;



        //pseudo-randomness

            random_device rd;
            mt19937 gen(rd());
            uniform_int_distribution<> dis(1, 5);



        for (int i=0;i<n;i++)
        {
            final_file_name=A[i]+format;
            file.open(final_file_name);
            r=dis(gen)%n;
            while(t==0)
            {
                if (A[i]!=B[r] && B[r].length()>0)
                {
                    file<<"["<<B[r]<<"]";
                    B[r].erase();
                    t=1;
                }
                else
                {
                    if (r<n-1)
                        r++;
                    else
                        r=0;
                }
            }
            file.close();
            t=0;
        }
        cout<<endl<<endl<<endl<<"The files have been created to the respective directory where the program exists. Send each file to its corresponding individual written on it. Inside each file exists the person he is going to gift."<<endl<<endl<<endl;
     }
     else
     {
         cout<<endl<<endl<<"An error occurred"<<endl;
         if (t==-1)
            cout<<"You inserted the same name more than once. The program will now exit"<<endl<<endl<<endl;
         else
            cout<<"You inserted less than 2 names. The program will now exit"<<endl<<endl<<endl;
     }
    cout<<"**************************"<<endl<<"Written shortly by XG-PRO"<<endl<<"**************************"<<endl;
    cout<<"Press any key and ENTER to exit the program..."<<endl;
    fflush(stdin);
    getchar();
    return 0;
}
