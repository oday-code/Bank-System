#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <cctype>
#include <fstream>
#include"mylib.h"

using namespace std;

void ShowMainMenueScreen();

void ShowTransactionsMenueScreen();

void ShowManageUsersScreen();

void Login();

void GoBack();

enum enChoose { ClientList = 1, Add = 2, Delete = 3, Update = 4, Find = 5, Transactions = 6, ManageMenue = 7, Logout = 8 };

enum enTranChoose { enDeposit = 1, enWithdraw = 2, enBakanceList = 3, enMainMenue = 4 };

enum enUserPermission { pShowClientsList = 1, pAddNewClient = 2, pDeleteClient = 4, pUpdateClient = 8, pFindClient = 16, pTransactions = 32, pManageUsers = 64 };

void MainMenue(short Choice);

struct stClient
{
	string AccountNumber = "";
	string PINCode = "";
	string Name = "";
	string Phone = "";
	double AccountBalance = 0.0;
	bool MarkToDelete = false;
};

struct stUser
{
	string Username = "";
	string Password = "";
	short userPermission = 0;
	bool MarkForDelete = false;
};

stUser CurrentUser;

struct stUserPermissions
{
	const short ShowClientsList = 1;
	const short AddNewClient = 2;
	const short DeleteClient = 4;
	const short UpdateClient = 8;
	const short FindClient = 16;
	const short Transactions = 32;
	const short ManageUsers = 64;
};

void ShowOptionTitle(string Title)
{
	cout << "===========================================\n";
	cout << "\t" << Title << endl;
	cout << "===========================================\n";
}

void AccessDenied()
{
	cout << "==============================================" << endl;
	cout << "Access Denied  : - ( " << endl;
	cout << "You Dont Have Permission To Do This." << endl;
	cout << "Please Connact Your Admin." << endl;
	cout << "==============================================" << endl;
}

vector<string> SplitString(string S1, string delim)
{
	short pos = 0;
	string sWord = "";
	vector<string> vWords;
	while ((pos = S1.find(delim)) != std::string::npos)
	{
		sWord = S1.substr(0, pos);
		if (sWord != " ")
		{
			vWords.push_back(sWord);
		}
		S1.erase(0, pos + delim.length());
	}

	if (S1 != "")
	{
		vWords.push_back(S1);
		return vWords;
	}
}

stClient ConvertLineClientToRecord(string Line)
{
	vector<string> vWords = SplitString(Line, "#//#");
	stClient ClientRecord;
	ClientRecord.AccountNumber = vWords[0];
	ClientRecord.PINCode = vWords[1];
	ClientRecord.Name = vWords[2];
	ClientRecord.Phone = vWords[3];
	ClientRecord.AccountBalance = stod(vWords[4]);
	return ClientRecord;
}

stUser ConvertLineUserToRecord(string Line)
{
	vector<string> vWords = SplitString(Line, "#//#");
	stUser UserRecord;
	UserRecord.Username = vWords[0];
	UserRecord.Password = vWords[1];
	UserRecord.userPermission = stoi(vWords[2]);
	return UserRecord;
}

vector<stClient> LoadLinesClientsFromFileToVector(string FileName)
{
	vector<stClient> vClientRecord;
	fstream MyFile;

	MyFile.open(FileName, ios::in);
	if (MyFile.is_open())
	{
		string Line;
		stClient ClientRecord;
		while (getline(MyFile, Line))
		{
			ClientRecord = ConvertLineClientToRecord(Line);
			vClientRecord.push_back(ClientRecord);
		}
		MyFile.close();
	}

	return vClientRecord;
}

vector<stUser> LoadLinesUsersFromFileToVector(string FileName)
{
	vector<stUser> vUserRecord;
	fstream MyFile;

	MyFile.open(FileName, ios::in);
	if (MyFile.is_open())
	{
		string Line;
		stUser UserRecord;
		while (getline(MyFile, Line))
		{
			UserRecord = ConvertLineUserToRecord(Line);
			vUserRecord.push_back(UserRecord);
		}
		MyFile.close();
	}

	return vUserRecord;
}

bool isClientExists(string AccountNumber, string FileName)
{
	vector<stClient> vClientsRecords = LoadLinesClientsFromFileToVector(FileName);

	for (stClient& C : vClientsRecords)
	{
		if (C.AccountNumber == AccountNumber)
		{
			return true;
		}
	}
	return false;
}

bool isUserExists(string UserName)
{
	vector<stUser>vUsersRecords = LoadLinesUsersFromFileToVector("Users.txt");
	for (stUser& U : vUsersRecords)
	{
		if (U.Username == UserName)
		{
			return true;
		}
	}
	return false;
}

stClient ReadClientRecord()
{
	stClient ClientRecord;
	ClientRecord.AccountNumber = read::ReadString("Enter Your Account Number?");

	while (isClientExists(ClientRecord.AccountNumber, "Clients.txt"))
	{
		cout << "Client With Account Number [" << ClientRecord.AccountNumber << "] Already Exists, Enter Another Account Number." << endl;
		ClientRecord.AccountNumber = read::ReadString("Enter Your Account Number?");
	}

	ClientRecord.PINCode = read::ReadString("Enter Your PIN Code?");
	ClientRecord.Name = read::ReadString("Enter Your Name?");
	ClientRecord.Phone = read::ReadString("Enter Your Phone?");
	ClientRecord.AccountBalance = read::ReadDoubleNumber("Eneter Your Account Balance?");

	return ClientRecord;
}

string ConvertRecordToLine(stClient ClientRecord, string Seperator)
{
	string Line = "";
	Line += ClientRecord.AccountNumber + Seperator;
	Line += ClientRecord.PINCode + Seperator;
	Line += ClientRecord.Name + Seperator;
	Line += ClientRecord.Phone + Seperator;
	Line += to_string(ClientRecord.AccountBalance);
	return Line;
}

string ConvertRecordToLine(stUser User, string Seperator)
{
	string Line = "";
	Line += User.Username + Seperator;
	Line += User.Password + Seperator;
	Line += to_string(User.userPermission);
	return Line;
}

void AddNewClient()
{
	fstream MyFile;
	stClient ClientRecord;
	string Line;
	MyFile.open("Clients.txt", ios::out | ios::app);
	ClientRecord = ReadClientRecord();
	Line = ConvertRecordToLine(ClientRecord, "#//#");
	if (MyFile.is_open())
	{
		MyFile << Line << endl;
		MyFile.close();
	}
}

void AddClient()
{
	char Answer = 'y';
	do
	{
		cout << "Adding New Client:\n " << endl;
		AddNewClient();
		cout << "Client Added Successfully!, Do You Want To Add More Client? Y/N? ";
		cin >> Answer;
		system("cls");
	} while (tolower(Answer) == 'y');

}

bool CheckUserPermission(enUserPermission UserPermission)
{
	if (CurrentUser.userPermission == -1)
		return true;
	if ((UserPermission & CurrentUser.userPermission) == UserPermission)
		return true;
	else
		return false;
}

void ShowAddClientScreen()
{
	if (CheckUserPermission(enUserPermission::pAddNewClient))
	{
		ShowOptionTitle("Add New Client Screen");
		AddClient();
	}
	else
	{
		system("cls");
		AccessDenied();
		GoBack();
	}
}

short UserPermissions()
{
	short UserPermission = 0;
	char Answer = 'y';

	cout << "Do You Want To Give Full Access? Y/N?" << endl;
	cin >> Answer;
	if (Answer == 'Y' || Answer == 'y')
	{
		UserPermission = -1;
	}

	else
	{
		cout << "Do You Want To Give Access To:" << endl;
		cout << "Show Clients List? Y/N?" << endl;
		cin >> Answer;
		if (Answer == 'Y' || Answer == 'y')
		{
			UserPermission += enUserPermission::pShowClientsList;
		}

		cout << "Add New Client? Y/N?" << endl;
		cin >> Answer;
		if (Answer == 'Y' || Answer == 'y')
		{
			UserPermission += enUserPermission::pAddNewClient;
		}

		cout << "Delete Client? Y/N?" << endl;
		cin >> Answer;
		if (Answer == 'Y' || Answer == 'y')
		{
			UserPermission += enUserPermission::pDeleteClient;
		}

		cout << "Update Client? Y/N?" << endl;
		cin >> Answer;
		if (Answer == 'Y' || Answer == 'y')
		{
			UserPermission += enUserPermission::pUpdateClient;
		}

		cout << "Find Client? Y/N?" << endl;
		cin >> Answer;
		if (Answer == 'Y' || Answer == 'y')
		{
			UserPermission += enUserPermission::pFindClient;
		}

		cout << "Transactions? Y/N?" << endl;
		cin >> Answer;
		if (Answer == 'Y' || Answer == 'y')
		{
			UserPermission += enUserPermission::pTransactions;
		}

		cout << "Manage Users? Y/N?" << endl;
		cin >> Answer;
		if (Answer == 'Y' || Answer == 'y')
		{
			UserPermission += enUserPermission::pManageUsers;
		}
	}
	return UserPermission;
}

stUser ReadUserRecord()
{
	stUser User;
	char Answer = 'y';

	User.Username = read::ReadString("Enter an Username? ");
	while (isUserExists(User.Username))
	{
		cout << "User With [" << User.Username << "] Already Exists, Enter Another UserName? " << endl;
		getline(cin >> ws, User.Username);
	}
	User.Password = read::ReadString("Enter a Password? ");
	User.userPermission = UserPermissions();
	return User;
}

void AddNewUser()
{
	fstream MyFile;
	stUser User = ReadUserRecord();
	string Line;
	MyFile.open("Users.txt", ios::out | ios::app);
	Line = ConvertRecordToLine(User, "#//#");
	if (MyFile.is_open())
	{
		MyFile << Line << endl;
		MyFile.close();
	}
}

void AddUser()
{
	char Answer = 'y';
	do
	{
		cout << "Adding User: " << endl;
		AddNewUser();
		cout << "User Added Succsessfully, Do You Want To Add More Users? Y/N?" << endl;
		cin >> Answer;
		system("cls");

	} while (Answer == 'Y' || Answer == 'y');
}

void ShowAddUserScreen()
{
	ShowOptionTitle("Add New User Screen");
	AddUser();
}

string ReadAccountNumber()
{
	string AN;
	cout << "\nEnter Your Account Number? ";
	getline(cin >> ws, AN);
	return AN;
}

bool FindClientByAccountNumber(string AccountNumber, vector<stClient>& vClientsRecords, stClient& Client)
{
	for (stClient C : vClientsRecords)
	{
		if (C.AccountNumber == AccountNumber)
		{
			Client = C;
			return true;
		}

	}
	return false;
}

bool FindUserByUserName(string UserName, vector<stUser>& vUserRecord, stUser& User)
{
	for (stUser& U : vUserRecord)
	{
		if (U.Username == UserName)
		{
			User = U;
			return true;
		}

	}
	return false;
}

void PrintClientCart(stClient Client)
{


	cout << "\nClient ID: " << endl;
	cout << "- - - - - - - - - - - - - - - - - - - - - - - - - - - " << endl;
	cout << "Account Number   :" << Client.AccountNumber << endl;
	cout << "PIN Code         :" << Client.PINCode << endl;
	cout << "Name             :" << Client.Name << endl;
	cout << "Phone            :" << Client.Phone << endl;
	cout << "Account Balance  :" << Client.AccountBalance << endl;
	cout << "- - - - - - - - - - - - - - - - - - - - - - - - - - - " << endl;
	cout << "\n\n";
}

void PrintUserCart(stUser User)
{
	cout << "\User ID: " << endl;
	cout << "- - - - - - - - - - - - - - - - - - - - - - - - - - - " << endl;
	cout << "UserName       :" << User.Username << endl;
	cout << "Password       :" << User.Password << endl;
	cout << "User Permission:" << User.userPermission << endl;
	cout << "- - - - - - - - - - - - - - - - - - - - - - - - - - - " << endl;
	cout << "\n\n";
}

void MarkForDeleteClient(vector<stClient>& vClientsRecords, string AccountNumber)
{
	for (stClient& C : vClientsRecords)
	{
		if (C.AccountNumber == AccountNumber)
		{
			C.MarkToDelete = true;
		}
	}
}

void MarkForDeleteUser(vector<stUser>& vUserRecord, string UserName)
{
	for (stUser& U : vUserRecord)
	{
		if (U.Username == UserName)
		{
			U.MarkForDelete = true;
		}
	}
}

void SaveClientsRecordsFromVectorToFile(vector<stClient> vClientsRecords)
{
	fstream MyFile;
	string Line;
	MyFile.open("Clients.txt", ios::out);
	if (MyFile.is_open())
	{
		for (stClient& C : vClientsRecords)
		{
			Line = ConvertRecordToLine(C, "#//#");
			if (C.MarkToDelete == false)
			{
				MyFile << Line << endl;
			}
		}
		MyFile.close();
	}
}

void SaveUsersRecordsFromVectorToFile(vector<stUser> vUserRecord)
{
	fstream MyFile;
	string Line;
	MyFile.open("Users.txt", ios::out);
	if (MyFile.is_open())
	{
		for (stUser& U : vUserRecord)
		{
			Line = ConvertRecordToLine(U, "#//#");
			if (U.MarkForDelete == false)
			{
				MyFile << Line << endl;
			}
		}
		MyFile.close();
	}

}

void GoBack()
{
	cout << "Press Any Key To Go Back To The Main Menue . . . " << endl;
	system("pause>0");
	system("cls");
	ShowMainMenueScreen();
}

void GoBackToTransactiosMenue()
{
	cout << "Press Any Key To Go Back To The Transactions Menue . . . " << endl;
	system("pause>0");
	system("cls");
	ShowTransactionsMenueScreen();
}

void GoBackToManageMenue()
{
	cout << "Press Any Key To Go Back To The Manage Menue . . . " << endl;
	system("pause>0");
	system("cls");
	ShowManageUsersScreen();
}

void DeleteClientByAccountNumber(vector<stClient>& vClientsRecords, string AccountNumber)
{
	char Answer = 'y';
	stClient Client;
	if (FindClientByAccountNumber(AccountNumber, vClientsRecords, Client))
	{
		PrintClientCart(Client);
		cout << "Are you Sure You Want To Delete This Client? Y/N? ";
		cin >> Answer;
		if (tolower(Answer) == 'y')
		{
			MarkForDeleteClient(vClientsRecords, AccountNumber);
			SaveClientsRecordsFromVectorToFile(vClientsRecords);
			vClientsRecords = LoadLinesClientsFromFileToVector("Clients.txt");
			cout << "Client Deleted Successfully!. " << endl;
		}
	}
	else
	{
		cout << "Client With Account Number (" << AccountNumber << ") Is Not Found!" << endl;
	}
}

void DeleteUserByUserName(vector<stUser>& vUsersRecords, string UserName)
{
	char Answer = 'y';
	stUser User;
	if (FindUserByUserName(UserName, vUsersRecords, User))
	{
		PrintUserCart(User);
		cout << "Are You Sure You Want To Delete This User? Y/N? ";
		cin >> Answer;
		if (tolower(Answer) == 'y')
		{
			MarkForDeleteUser(vUsersRecords, UserName);
			SaveUsersRecordsFromVectorToFile(vUsersRecords);
			vUsersRecords = LoadLinesUsersFromFileToVector("Users.txt");
			cout << "User Deleted Successfully." << endl;
		}
	}
	else
	{
		cout << "User By UserName [" << UserName << "] Is Not Found." << endl;
	}
}

void ShowDeleteClientScreen()
{
	if (CheckUserPermission(enUserPermission::pDeleteClient))
	{
		ShowOptionTitle("Delete Client Screen");
		vector<stClient> vClientsRecords = LoadLinesClientsFromFileToVector("Clients.txt");
		string AccountNumber = ReadAccountNumber();
		DeleteClientByAccountNumber(vClientsRecords, AccountNumber);
	}
	else
	{
		system("cls");
		AccessDenied();
		GoBack();
	}
}

void ShowDeleteUserScreen()
{
	ShowOptionTitle("Delete User Screen");
	vector<stUser> vUsersRecords = LoadLinesUsersFromFileToVector("Users.txt");
	string UserName = read::ReadString("Enter UserName? ");
	DeleteUserByUserName(vUsersRecords, UserName);
}

stClient ChangeClientRecord(string AccountNumber)
{
	stClient C;
	C.AccountNumber = AccountNumber;
	C.PINCode = read::ReadString("Enter Your New PIN Code?");
	C.Name = read::ReadString("Enter Your New Name?");
	C.Phone = read::ReadString("Enter Your New Phone Number?");
	C.AccountBalance = read::ReadDoubleNumber("Enter Your New Account Balance?");
	return C;
}

stUser ChangeUserRecord(string UserName)
{
	stUser U;
	U.Username = UserName;
	U.Password = read::ReadString("Enter Your A New Password? ");
	U.userPermission = UserPermissions();
	return U;
}

void UpdateClientInfoByAccountNumber(vector<stClient>& vClientsRecords, string AccountNumber)
{
	char Answer = 'y';
	stClient Client;
	if (FindClientByAccountNumber(AccountNumber, vClientsRecords, Client))
	{
		PrintClientCart(Client);
		cout << "Are you Sure You Want To Update Info This Client? Y/N? ";
		cin >> Answer;
		cout << "\n";
		if (tolower(Answer) == 'y')
		{
			for (stClient& C : vClientsRecords)
			{
				if (C.AccountNumber == AccountNumber)
				{
					C = ChangeClientRecord(AccountNumber);
					break;
				}
			}
			SaveClientsRecordsFromVectorToFile(vClientsRecords);
			cout << "\nClient updated Successfully.\n" << endl;
		}
	}
	else
	{
		cout << "Client With Account Number (" << AccountNumber << ") Is Not Found!" << endl;
	}
}

void UpdateUserInfoByUserName(vector<stUser>& vUsersRecords, string UserName)
{
	char Answer = 'y';
	stUser User;
	if (FindUserByUserName(UserName, vUsersRecords, User))
	{
		PrintUserCart(User);
		cout << "Are you Sure You Want To Update Info This User? Y/N? ";
		cin >> Answer;
		cout << "\n";
		if (tolower(Answer) == 'y')
		{
			for (stUser& U : vUsersRecords)
			{
				if (U.Username == UserName)
				{
					U = ChangeUserRecord(UserName);
					break;
				}
			}
			SaveUsersRecordsFromVectorToFile(vUsersRecords);
			cout << "\User updated Successfully.\n" << endl;
		}
	}
	else
	{
		cout << "User With Account Number (" << UserName << ") Is Not Found!" << endl;
	}
}

void ShowUpdateClientScreen()
{
	if (CheckUserPermission(enUserPermission::pUpdateClient))
	{
		ShowOptionTitle("Update Client Screen");
		vector<stClient> vClientsRecords = LoadLinesClientsFromFileToVector("Clients.txt");
		string AccountNumber = ReadAccountNumber();
		UpdateClientInfoByAccountNumber(vClientsRecords, AccountNumber);
	}
	else
	{
		system("cls");
		AccessDenied();
		GoBack();
	}
}

void ShowUpdateUsersScreen()
{
	ShowOptionTitle("Update User Screen");
	vector<stUser> vUsersRecords = LoadLinesUsersFromFileToVector("Users.txt");
	string UserName = read::ReadString("Enter UserName? ");
	UpdateUserInfoByUserName(vUsersRecords, UserName);
}

void FindClient(string AccountNumber)
{
	vector<stClient> vClientsRecords = LoadLinesClientsFromFileToVector("Clients.txt");
	stClient Client;
	if (FindClientByAccountNumber(AccountNumber, vClientsRecords, Client))
	{
		PrintClientCart(Client);
	}
	else
	{
		cout << "Client With Account Number (" << AccountNumber << ") Is Not Found!" << endl;
	}
}

void FindUser(string UserName)
{
	vector<stUser> vUsersRecords = LoadLinesUsersFromFileToVector("Users.txt");
	stUser User;
	if (FindUserByUserName(UserName, vUsersRecords, User))
	{
		PrintUserCart(User);
	}
	else
	{
		cout << "User By UserName [" << UserName << "] Is Not Found!" << endl;
	}
}

bool FindUserByUsernameAndPassword(string Username, string Password, stUser& User)
{
	vector <stUser> vUsers = LoadLinesUsersFromFileToVector("Users.txt");
	for (stUser U : vUsers)
	{
		if (U.Username == Username && U.Password == Password)
		{
			User = U;
			return true;
		}
	}
	return false;
}

void ShowFindClientScreen()
{
	if (CheckUserPermission(enUserPermission::pFindClient))
	{
		ShowOptionTitle("Find Client Screen");
		string AccountNumber = ReadAccountNumber();
		FindClient(AccountNumber);
	}
	else
	{
		system("cls");
		AccessDenied();
		GoBack();
	}
}

void ShowFindUserScreen()
{
	ShowOptionTitle("Find User Screen");
	string UserName = read::ReadString("Enter an User Name? ");
	FindUser(UserName);
}

void ProgrammEnds()
{
	cout << "_______________________________________________________________________________________________________" << endl;
	cout << "                                 P r o g r a m      E n d s     : - )" << endl;
	cout << "_______________________________________________________________________________________________________\n\n" << endl;
}

void PrintClientRecord(stClient Client)
{
	cout << " |" << left << setw(20) << Client.AccountNumber;
	cout << " |" << left << setw(20) << Client.PINCode;
	cout << " |" << left << setw(21) << Client.Name;
	cout << " |" << left << setw(20) << Client.Phone;
	cout << " |" << left << setw(20) << Client.AccountBalance;
	cout << endl;
}

void PrintClientsList()
{
	vector<stClient> vClientsRecords = LoadLinesClientsFromFileToVector("Clients.txt");
	cout << "\t\t\t\t\t    Clients List (" << vClientsRecords.size() << ") Client(s)." << endl;
	cout << "_______________________________________________________";
	cout << "_______________________________________________________" << endl;
	cout << " |" << left << setw(20) << "Account Number";
	cout << " |" << left << setw(20) << "PIN Code";
	cout << " |" << left << setw(21) << "Name";
	cout << " |" << left << setw(20) << "Phone";
	cout << " |" << left << setw(20) << "Account Balance" << endl;
	cout << "_______________________________________________________";
	cout << "_______________________________________________________" << endl;
	for (stClient& C : vClientsRecords)
	{
		PrintClientRecord(C);
	}
	cout << "\n\n";
}

void PrintUserRecord(stUser User)
{
	cout << " |" << left << setw(30) << User.Username;
	cout << " |" << left << setw(30) << User.Password;
	cout << " |" << left << setw(15) << User.userPermission;
	cout << endl;
}

void PrintUsersList()
{
	vector<stUser> vUsersRecords = LoadLinesUsersFromFileToVector("Users.txt");
	cout << "\t\t\t\t\t    Users List (" << vUsersRecords.size() << ") User(s)." << endl;
	cout << "_______________________________________________________";
	cout << "_______________________________________________________" << endl;
	cout << " |" << left << setw(30) << "Username";
	cout << " |" << left << setw(30) << "Password";
	cout << " |" << left << setw(15) << "User Permission" << endl;
	cout << "_______________________________________________________";
	cout << "_______________________________________________________" << endl;
	for (stUser& C : vUsersRecords)
	{
		PrintUserRecord(C);
	}
	cout << "\n\n";
}

void ShowAllClientsScreen()
{
	if (CheckUserPermission(enUserPermission::pShowClientsList))
	{
		ShowOptionTitle("Client List Screen");
		PrintClientsList();
	}
	else
	{
		system("cls");
		AccessDenied();
		GoBack();
	}

}

void ShowAllUsersScreen()
{
	ShowOptionTitle("Users List Screen");
	PrintUsersList();
}

void ShowOptions()
{
	cout << "   [1] Show Client List." << endl;
	cout << "   [2] Add New Client." << endl;
	cout << "   [3] Delete Client." << endl;
	cout << "   [4] Update Client." << endl;
	cout << "   [5] Find Client." << endl;
	cout << "   [6] Transactions." << endl;
	cout << "   [7] Manage Users." << endl;
	cout << "   [8] Logout." << endl;
	cout << "===========================================\n\n" << endl;

}

void DepositClientBalanceByAccountNumber(string AccountNumber, vector<stClient>& vClientsRecords, double Amount)
{
	char Answer = 'y';
	cout << "Are You Sure You Want To Perform This Transaction? Y/N? ";
	cin >> Answer;
	if (tolower(Answer) == 'y')
	{
		for (stClient& C : vClientsRecords)
		{
			if (C.AccountNumber == AccountNumber)
			{
				C.AccountBalance = C.AccountBalance + Amount;
				SaveClientsRecordsFromVectorToFile(vClientsRecords);
				cout << "Done Succseefully. New Balance Is: " << C.AccountBalance << "." << endl;
				break;
			}
		}
	}

}

void Deposit(vector<stClient>& vClientsRecord)
{

	stClient Client;
	string AccountNumber = ReadAccountNumber();
	double DepositAmount = 0;

	while (!FindClientByAccountNumber(AccountNumber, vClientsRecord, Client))
	{
		cout << "\nClient With Account Number [" << AccountNumber << "] Doe No Exite." << endl;
		AccountNumber = ReadAccountNumber();
	}

	PrintClientCart(Client);

	cout << "Please Enter Deposit Amount? ";
	cin >> DepositAmount;

	DepositClientBalanceByAccountNumber(AccountNumber, vClientsRecord, DepositAmount);
}

void DepositTranaction()
{
	vector<stClient> vClientsRecords = LoadLinesClientsFromFileToVector("Clients.txt");
	Deposit(vClientsRecords);
}

void ShowDepositScreen()
{
	ShowOptionTitle("Deposit Screen");
	DepositTranaction();
}

void Withdraw(vector<stClient>& vClientsRecords)
{
	string AccountNumber;
	stClient Client;
	AccountNumber = read::ReadString("Enter Your Account Number?");

	while (!FindClientByAccountNumber(AccountNumber, vClientsRecords, Client))
	{
		cout << "\nClient With Account Number [" << AccountNumber << "] Dose No Exite." << endl;
		AccountNumber = read::ReadString("Enter Your Account Number?");
	}
	PrintClientCart(Client);

	double WithdrawAmount = read::ReadDoubleNumber("\nEnter Your Withdraw Amount?");

	for (stClient& C : vClientsRecords)
	{
		while (C.AccountBalance < WithdrawAmount)
		{
			cout << "Amount Exceed The Balance, You Can Withdraw Up To: " << C.AccountBalance << "." << endl;
			WithdrawAmount = read::ReadDoubleNumber("\nEnter Another Amount?");
		}
	}

	DepositClientBalanceByAccountNumber(AccountNumber, vClientsRecords, WithdrawAmount * -1);

}

void WithdrawTransaction()
{
	vector<stClient> vClientsRecords = LoadLinesClientsFromFileToVector("Clients.txt");
	Withdraw(vClientsRecords);

}

void ShowWithdrawScreen()
{
	ShowOptionTitle("Withdraw Screen");
	WithdrawTransaction();

}

void PrintBalanceList(stClient Client)
{
	cout << " |" << left << setw(35) << Client.AccountNumber;
	cout << " |" << left << setw(35) << Client.Name;
	cout << " |" << left << setw(35) << Client.AccountBalance;

}

void BalanceList()
{
	vector<stClient> vClientsRecords = LoadLinesClientsFromFileToVector("Clients.txt");
	cout << "\t\t\t\t\t    Balance List(" << vClientsRecords.size() << ") Client(s)." << endl;
	cout << "_______________________________________________________";
	cout << "_______________________________________________________" << endl;
	cout << " |" << left << setw(35) << "Account Number";
	cout << " |" << left << setw(35) << "Client Name";
	cout << " |" << left << setw(35) << "Balance" << endl;
	cout << "_______________________________________________________";
	cout << "_______________________________________________________" << endl;
	double TotalBalance = 0;
	for (stClient& C : vClientsRecords)
	{
		PrintBalanceList(C);
		TotalBalance += C.AccountBalance;
		cout << endl;
	}
	cout << "\n\t\t\t\t\t    Total Balances = " << TotalBalance << endl;

}

void ShowBalanceScreen()
{
	ShowOptionTitle("Balance Screen");
	BalanceList();

}

short ChooseOption()
{
	short Choice;
	cout << "Choose What Do You Want To Do? [ 1 To 8 ] ?";
	cin >> Choice;
	return Choice;

}

void TransactionsOption()
{
	cout << "   [1] Deposit." << endl;
	cout << "   [2] Withdraw." << endl;
	cout << "   [3] Total Balances." << endl;
	cout << "   [4] Main Menue." << endl;
	cout << "===========================================\n\n" << endl;
}

short ChooseTransactionsOption()
{
	short Choice;
	cout << "Choose What Do You Want To Do? [ 1 To 4 ] ?";
	cin >> Choice;
	return Choice;
}

void TransactionsMenue(short TransactionsOption)
{
	switch (TransactionsOption)
	{

	case enTranChoose::enDeposit:
	{
		system("cls");
		ShowDepositScreen();
		GoBackToTransactiosMenue();
		break;
	}

	case enTranChoose::enWithdraw:
	{
		system("cls");
		ShowWithdrawScreen();
		GoBackToTransactiosMenue();
		break;
	}

	case enTranChoose::enBakanceList:
	{
		system("cls");
		ShowBalanceScreen();
		GoBackToTransactiosMenue();
		break;
	}

	case enTranChoose::enMainMenue:
	{
		system("cls");
		ShowMainMenueScreen();
		break;
	}
	}
}

void ShowTransactionsMenueScreen()
{
	if (CheckUserPermission(enUserPermission::pTransactions))
	{
		ShowOptionTitle("Transactions Menue Screen");
		TransactionsOption();
		TransactionsMenue(ChooseTransactionsOption());
	}
	else
	{
		system("cls");
		AccessDenied();
		GoBack();
	}
}

void ManageUsersOptions()
{
	cout << "   [1]  Users List." << endl;
	cout << "   [2]  Add New Users." << endl;
	cout << "   [3]  Delete Users." << endl;
	cout << "   [4]  Update Users." << endl;
	cout << "   [5]  Find Users." << endl;
	cout << "   [6]  Main Manue." << endl;
	cout << "===========================================\n\n" << endl;
}

short ChooseManageUsersOptions()
{
	short Choice;
	cout << "Choose What Do You Want To Do? [ 1 To 6 ] ?";
	cin >> Choice;
	return Choice;
}

enum enManageMenue { eUsersList = 1, eAddNewUsers = 2, eDeleteUsers = 3, eUpdateUsers = 4, eFindUsers = 5, eMainMenue = 6 };

void ManageUsersMenue(short Choice)
{
	switch (Choice)
	{
	case enManageMenue::eUsersList:
	{
		system("cls");
		ShowAllUsersScreen();
		GoBackToManageMenue();
		break;
	}
	case enManageMenue::eAddNewUsers:
	{
		system("cls");
		ShowAddUserScreen();
		GoBackToManageMenue();
		break;
	}
	case enManageMenue::eDeleteUsers:
	{
		system("cls");
		ShowDeleteUserScreen();
		GoBackToManageMenue();
		break;
	}
	case enManageMenue::eUpdateUsers:
	{
		system("cls");
		ShowUpdateUsersScreen();
		GoBackToManageMenue();
		break;
	}
	case enManageMenue::eFindUsers:
	{
		system("cls");
		ShowFindUserScreen();
		GoBackToManageMenue();
		break;
	}
	case enManageMenue::eMainMenue:
	{
		system("cls");
		ShowMainMenueScreen();
		GoBackToManageMenue();
		break;
	}

	}
}

void ShowManageUsersScreen()
{
	if (CheckUserPermission(enUserPermission::pShowClientsList))
	{
		ShowOptionTitle("Manage Users Screen ");
		ManageUsersOptions();
		short Choice = ChooseManageUsersOptions();
		ManageUsersMenue(Choice);
	}
	else
	{
		system("cls");
		AccessDenied();
		GoBack();
	}
}


void MainMenue(short Choice)
{

	switch (Choice)
	{
	case enChoose::ClientList:
		system("cls");
		ShowAllClientsScreen();
		GoBack();
		break;

	case enChoose::Add:
		system("cls");
		ShowAddClientScreen();
		GoBack();
		break;

	case enChoose::Delete:
		system("cls");
		ShowDeleteClientScreen();
		GoBack();
		break;

	case enChoose::Update:
		system("cls");
		ShowUpdateClientScreen();
		GoBack();
		break;

	case enChoose::Find:
		system("cls");
		ShowFindClientScreen();
		GoBack();
		break;

	case enChoose::Transactions:
		system("cls");
		ShowTransactionsMenueScreen();
		break;

	case enChoose::ManageMenue:
		system("cls");
		ShowManageUsersScreen();
		break;

	case enChoose::Logout:
		system("cls");
		Login();
		break;

	}
}

void ShowMainMenueScreen()
{
	ShowOptionTitle("Main Menue Screen");
	ShowOptions();
	MainMenue(ChooseOption());
}


void Login()
{
	string UserName = "";
	string Password = "";
	bool LoginFaild = false;
	do
	{
		system("cls");
		ShowOptionTitle("Login Screen");
		if (LoginFaild)
		{
			cout << "Invalid UserName/Password!" << endl;
		}

		UserName = read::ReadString("Enter UserName?");
		Password = read::ReadString("Enter Password?");

		if (FindUserByUsernameAndPassword(UserName, Password, CurrentUser))
			LoginFaild = false;
		else
			LoginFaild = true;

	} while (LoginFaild);

	ShowMainMenueScreen();

}
int main()
{
	Login();
	return 0;
}