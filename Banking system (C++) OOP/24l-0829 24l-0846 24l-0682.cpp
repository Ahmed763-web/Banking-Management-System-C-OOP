#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <cstdio>
#include <cctype>

using namespace std;



const string FILE_USERS = "users.txt";
const string FILE_COMPANIES = "companies.txt";
const string FILE_EMPLOYEES_LINK = "companies_employees.txt";
const string FILE_TRANS = "transactions.txt";
const string FILE_CARDS = "cards.txt";
const string FILE_ADMIN = "admin.txt";
const string FILE_LOANS = "loans.txt";
const string FILE_FRAUD = "fraud.txt";

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

class Validator {
private:
    static const int MAX_NAME_LENGTH = 50;
    static const int MIN_NAME_LENGTH = 3;
    static const int PHONE_LENGTH = 11;
    static const int CNIC_LENGTH = 13;
    static const int MIN_PASSWORD_LENGTH = 8;
    static const int MAX_ADDRESS_LENGTH = 100;

public:
    static bool namevalid(const string& name) {
        if (name.length() < MIN_NAME_LENGTH || name.length() > MAX_NAME_LENGTH)
            return false;

        for (char c : name) {
            if (!isalpha(c) && !isspace(c) && c != '.')
                return false;
        }

        int spaceCount = 0;
        for (char c : name) {
            if (isspace(c)) spaceCount++;
        }
        return spaceCount >= 1;
    }

    static bool validaddress(const string& address) {
        if (address.length() < 10 || address.length() > MAX_ADDRESS_LENGTH)
            return false;
        return true;
    }

    static bool validphone(const string& phone) {
        if (phone.length() != PHONE_LENGTH)
            return false;

        if (!isnumeric(phone))
            return false;

        string prefix = phone.substr(0, 2);
        return prefix == "03" || prefix == "04";
    }

    static bool isvalidCNIC(const string& cnic) {
        return cnic.length() == CNIC_LENGTH && isnumeric(cnic);
    }

    static bool validPassword(const string& password) {
        if (password.length() < MIN_PASSWORD_LENGTH)
            return false;

        return hasuppercase(password) &&
            hasLowercase(password) &&
            hasSpecialchar(password);
    }

    static bool validloginid(const string& loginID) {
        if (loginID.length() < 5)
            return false;

        for (char c : loginID) {
            if (!isalnum(c))
                return false;
        }
        return true;
    }

    static bool isnumeric(const string& str) {
        for (char c : str) {
            if (!isdigit(c))
                return false;
        }
        return true;
    }

    static bool hasSpecialchar(const string& str) {
        string specialChars = "!@#$%^&*()_+-=[]{}|;:,.<>?";
        for (char c : str) {
            for (char sc : specialChars) {
                if (c == sc) return true;
            }
        }
        return false;
    }

    static bool hasuppercase(const string& str) {
        for (char c : str) {
            if (isupper(c))
                return true;
        }
        return false;
    }

    static bool hasLowercase(const string& str) {
        for (char c : str) {
            if (islower(c))
                return true;
        }
        return false;
    }
};

// Helper
string getcurrentdate() {
    time_t now = time(0);
    tm* ltm = localtime(&now);
    if (ltm == NULL) return "01-01-2025";

    char buf[20];
    sprintf(buf, "%02d-%02d-%04d", ltm->tm_mday, ltm->tm_mon + 1, ltm->tm_year + 1900);
    return string(buf);
}

string trim(string s) {
    size_t start = s.find_first_not_of(" \t\n\r");
    size_t end = s.find_last_not_of(" \t\n\r");
    return (start == string::npos) ? "" : s.substr(start, end - start + 1);
}

int getSafeInt() {
    int val;
    while (!(cin >> val)) {
        cout << "Invalid input! Enter a number: ";
        cin.clear();
        cin.ignore(10000, '\n');
    }
    cin.ignore();
    return val;
}

double getSafeDouble() {
    double val;
    while (!(cin >> val)) {
        cout << "Invalid input! Enter amount: ";
        cin.clear();
        cin.ignore(10000, '\n');
    }
    cin.ignore();
    return val;
}

string getString() {
    string s;
    getline(cin, s);
    return trim(s);
}

string getfield(string line, int fieldIndex) {
    string fields[20];
    int fieldCount = 0;
    string currentField = "";

    for (size_t i = 0; i < line.length(); i++) {
        if (line[i] == ',') {
            fields[fieldCount++] = trim(currentField);
            currentField = "";
        }
        else {
            currentField += line[i];
        }
    }
    fields[fieldCount++] = trim(currentField);

    return (fieldIndex < fieldCount) ? fields[fieldIndex] : "";
}

long long generatecardnumber() {
    long long base = 4000000000000000LL;
    long long random = (long long)rand() * 1000000 + (long long)rand();
    return base + (random % 1000000000000000LL);
}

void displayAccounttypeInfo(string clientType) {
    cout << "\n=== ACCOUNT TYPE INFORMATION ===\n";
    if (clientType == "user") {
        cout << "Bronze Account: Daily Limit = 100,000 PKR\n";
        cout << "Gold Account: Daily Limit = 500,000 PKR\n";
        cout << "Users cannot open Business accounts\n";
    }
    else {
        cout << "Bronze Account: Daily Limit = 100,000 PKR\n";
        cout << "Gold Account: Daily Limit = 500,000 PKR\n";
        cout << "Business Account: Daily Limit = 20,000,000 PKR\n";
    }
    cout << "==============\n\n";
}

string formatLargeNumber(double number) {
    stringstream ss;
    ss << fixed << setprecision(0) << number;
    return ss.str();
}

class AccountType {
protected:
    string name;
    double dailyLimit;
public:
    AccountType(string n, double l) : name(n), dailyLimit(l) {}
    virtual ~AccountType() {}
    virtual string getName() { return name; }
    virtual double getDailyLimit() { return dailyLimit; }
    virtual void displayInfo() = 0;
    virtual bool canUserHave() = 0;
    virtual bool canCompanyHave() = 0;
};

class Bronze : public AccountType {
public:
    Bronze() : AccountType("Bronze", 100000.0) {}
    void displayInfo() override { cout << "Bronze Account (Daily Limit: 100,000 PKR)"; }
    bool canUserHave() override { return true; }
    bool canCompanyHave() override { return true; }
};

class Gold : public AccountType {
public:
    Gold() : AccountType("Gold", 500000.0) {}
    void displayInfo() override { cout << "Gold Account (Daily Limit: 500,000 PKR)"; }
    bool canUserHave() override { return true; }
    bool canCompanyHave() override { return true; }
};

class Business : public AccountType {
public:
    Business() : AccountType("Business", 20000000.0) {}
    void displayInfo() override { cout << "Business Account (Daily Limit: 20,000,000 PKR)"; }
    bool canUserHave() override { return false; }  // bussiness accounts only for companies
    bool canCompanyHave() override { return true; }
};

// card class
class Card {
private:
    long long cardNumber;
    string pin;
    bool isActive;
    int failedAttempts;
public:
    Card(long long num, string p) : cardNumber(num), pin(p), isActive(true), failedAttempts(0) {}

    long long getNumber() { return cardNumber; }
    bool isCardActive() { return isActive; }
    void deactivate() { isActive = false; }

    bool verifyPIN(string enteredPIN) {
        if (!isActive) return false;

        if (enteredPIN == pin) {
            failedAttempts = 0;
            return true;
        }
        else {
            failedAttempts++;
            if (failedAttempts >= 3) {
                deactivate();
            }
            return false;
        }
    }

    int getFailedAttempts() { return failedAttempts; }
};

// base client class (inheritance + abstraction) dono included hain
class Client {
protected:
    int id;
    string loginId;
    string password;
    double balance;
    bool approved;
    bool frozen;
public:
    Client(int i, string lid, string p, double bal, bool appr)
        : id(i), loginId(lid), password(p), balance(bal), approved(appr), frozen(false) {
    }
    virtual ~Client() {}

    // Getters
    int getId() { return id; }
    string getLoginId() { return loginId; }
    bool checkPassword(string p) { return password == p; }
    double getBalance() { return balance; }
    bool isApproved() { return approved; }
    bool isfrozen() { return frozen; }

    // Setters
    void setApproved(bool a) { approved = a; }
    void setfrozen(bool f) { frozen = f; }
    void setBalance(double b) { balance = b; }
    void addBalance(double amount) { balance += amount; }
    bool subtractBalance(double amount) {
        if (amount <= balance) {
            balance -= amount;
            return true;
        }
        return false;
    }

    // Pure virtual functions
    virtual string getName() = 0;
    virtual string getType() = 0;
    virtual void displayMenu() = 0;
    virtual AccountType* getAccountType() = 0;
    virtual double getDailyWithdrawn() = 0;
    virtual void addDailyWithdrawn(double amount) = 0;
    virtual void resetDailyLimit() = 0;
};

// user client class
class UserClient : public Client {
private:
    string fullName;
    string address;
    string phone;
    string cnic;

    Card** cards; //  Cards are part of UserClient
    int cardCount;
    int cardCapacity;

    double dailyWithdrawn;
    AccountType* accountType; // Association

public:
    string getPassword() { return password; }
    UserClient(int uid, string name, string addr, string ph, string cn,
        string lid, string pass, double bal, AccountType* accType, bool appr)
        : Client(uid, lid, pass, bal, appr), fullName(name), address(addr),
        phone(ph), cnic(cn), dailyWithdrawn(0), accountType(accType)
    {
        cardCount = 0;
        cardCapacity = 5;
        cards = new Card * [cardCapacity];
        for (int i = 0; i < cardCapacity; i++) cards[i] = NULL;
    }

    ~UserClient() {
        for (int i = 0; i < cardCount; i++) {
            if (cards[i]) delete cards[i]; // cards are destroyed with user
        }
        delete[] cards;
        if (accountType) delete accountType;
    }

    string getName() override { return fullName; }
    string getType() override { return "User"; }
    string getAddress() { return address; }
    string getPhone() { return phone; }
    string getCNIC() { return cnic; }
    AccountType* getAccountType() override { return accountType; }

    void addCard(Card* card) {
        if (cardCount < cardCapacity) {
            cards[cardCount++] = card;
        }
    }

    int getCardCount() { return cardCount; }
    Card* getCard(int index) {
        return (index >= 0 && index < cardCount) ? cards[index] : NULL;
    }

    bool hasActiveCards() {
        for (int i = 0; i < cardCount; i++) {
            if (cards[i] && cards[i]->isCardActive()) return true;
        }
        return false;
    }

    void resetDailyLimit() override { dailyWithdrawn = 0; }
    double getDailyWithdrawn() override { return dailyWithdrawn; }
    void addDailyWithdrawn(double amount) override { dailyWithdrawn += amount; }

    bool canWithdraw(double amount) {
        if (frozen) {
            cout << "[ERROR] Account is frozen!\n";
            return false;
        }
        if (!approved) {
            cout << "[ERROR] Account not approved yet!\n";
            return false;
        }
        if (amount > balance) {
            cout << "[ERROR] Insufficient balance!\n";
            return false;
        }
        if (dailyWithdrawn + amount > accountType->getDailyLimit()) {
            cout << "[ERROR] Daily withdrawal limit exceeded! ";
            cout << "Remaining today: " << formatLargeNumber(accountType->getDailyLimit() - dailyWithdrawn) << " PKR\n";
            return false;
        }
        return true;
    }

    void displayMenu() override {
        cout << "\nUSER CLIENT DASHBOARD \n";
        cout << "Welcome, " << fullName << "!\n";
        cout << "Account Type: "; accountType->displayInfo(); cout << "\n";
        cout << "Balance: " << formatLargeNumber(balance) << " PKR\n";
        cout << "Daily Withdrawn: " << formatLargeNumber(dailyWithdrawn) << " / " << formatLargeNumber(accountType->getDailyLimit()) << " PKR\n";
        if (frozen) cout << "*** ACCOUNT FROZEN ***\n";

        cout << "\n1. Deposit Money\n";
        cout << "2. Withdraw Money\n";
        cout << "3. Transfer Money\n";
        cout << "4. View Transaction History\n";
        cout << "5. View Account Details\n";
        cout << "6. Logout\n";
        cout << "Select option: ";
    }
};

// inheriting
class CompanyClient : public Client {
private:
    string companyName;
    string address;
    string taxNumber;

    double dailyWithdrawn;
    AccountType* accountType; // Association

public:
    string getPassword() { return password; }
    CompanyClient(int cid, string name, string addr, string tax,
        string lid, string pass, double bal, AccountType* accType, bool appr)
        : Client(cid, lid, pass, bal, appr), companyName(name), address(addr),
        taxNumber(tax), dailyWithdrawn(0), accountType(accType) {
    }

    ~CompanyClient() {
        if (accountType) delete accountType;
    }

    string getName() override { return companyName; }
    string getType() override { return "Company"; }
    string getAddress() { return address; }
    string getTaxNumber() { return taxNumber; }
    AccountType* getAccountType() override { return accountType; }

    void resetDailyLimit() override { dailyWithdrawn = 0; }
    double getDailyWithdrawn() override { return dailyWithdrawn; }
    void addDailyWithdrawn(double amount) override { dailyWithdrawn += amount; }

    bool canWithdraw(double amount) {
        if (frozen) {
            cout << "[ERROR] Company account is frozen!\n";
            return false;
        }
        if (!approved) {
            cout << "[ERROR] Company account not approved yet!\n";
            return false;
        }
        if (amount > balance) {
            cout << "[ERROR] Insufficient balance!\n";
            return false;
        }
        if (dailyWithdrawn + amount > accountType->getDailyLimit()) {
            cout << "[ERROR] Daily withdrawal limit exceeded! ";
            cout << "Remaining today: " << formatLargeNumber(accountType->getDailyLimit() - dailyWithdrawn) << " PKR\n";
            return false;
        }
        return true;
    }

    void displayMenu() override {
        cout << "\n=== COMPANY CLIENT DASHBOARD ===\n";
        cout << "Welcome, " << companyName << "!\n";
        cout << "Account Type: "; accountType->displayInfo(); cout << "\n";
        cout << "Balance: " << formatLargeNumber(balance) << " PKR\n";
        cout << "Daily Withdrawn: " << formatLargeNumber(dailyWithdrawn) << " / " << formatLargeNumber(accountType->getDailyLimit()) << " PKR\n";
        if (frozen) cout << "*** ACCOUNT FROZEN ***\n";

        cout << "\n1. Deposit Money\n";
        cout << "2. Withdraw Money\n";
        cout << "3. Transfer to User\n";
        cout << "4. Request Loan\n";
        cout << "5. View Transaction History\n";
        cout << "6. View Company Details\n";
        cout << "7. Logout\n";
        cout << "Select option: ";
    }
};


class BankEmployee {
private:
    int id;
    string loginId;
    string password;
    string name;

public:
    BankEmployee(int eid, string lid, string pass, string n = "Bank Employee")
        : id(eid), loginId(lid), password(pass), name(n) {
    }

    int getId() { return id; }
    string getLoginId() { return loginId; }
    bool checkPassword(string p) { return password == p; }
    string getName() { return name; }

    void displayMenu() {
        cout << "\n=== BANK EMPLOYEE DASHBOARD ===\n";
        cout << "Welcome, " << name << "!\n";

        cout << "\n1. View All Accounts\n";
        cout << "2. Approve/Reject User Accounts\n";
        cout << "3. Approve/Reject Company Accounts\n";
        cout << "4. Manage Loan Requests\n";
        cout << "5. View Transaction History\n";
        cout << "6. Freeze/Unfreeze Accounts\n";
        cout << "7. Reset Daily Limits\n";
        cout << "8. Logout\n";
        cout << "Select option: ";
    }
};

//  main class (aggregation + asoociation)
class BankSystem {
private:
    UserClient** users;
    int userCount;
    int userCapacity;

    CompanyClient** companies;
    int companyCount;
    int companyCapacity;

    BankEmployee** employees;
    int employeeCount;
    int employeeCapacity;

    // employee-company associations
    struct EmployeeLink {
        int userId;
        int companyId;
    };
    EmployeeLink* employeeLinks;
    int linkCount;
    int linkCapacity;

public:
    BankSystem() {
        userCount = companyCount = employeeCount = linkCount = 0;
        userCapacity = companyCapacity = employeeCapacity = linkCapacity = 10;

        users = new UserClient * [userCapacity];
        companies = new CompanyClient * [companyCapacity];
        employees = new BankEmployee * [employeeCapacity];
        employeeLinks = new EmployeeLink[linkCapacity];

        loadAllData();
        ensureDefaultAdmin();
    }

    ~BankSystem() {
        for (int i = 0; i < userCount; i++) delete users[i];
        for (int i = 0; i < companyCount; i++) delete companies[i];
        for (int i = 0; i < employeeCount; i++) delete employees[i];

        delete[] users;
        delete[] companies;
        delete[] employees;
        delete[] employeeLinks;
    }

private:
    void ensureDefaultAdmin() {
        if (employeeCount == 0) {
            cout << "[SYSTEM] Creating default admin account...\n";
            employees[employeeCount++] = new BankEmployee(1, "admin", "admin123", "System Administrator");

            ofstream adminFile(FILE_ADMIN.c_str());
            if (adminFile) {
                adminFile << "1,admin,admin123,System Administrator\n";
                adminFile.close();
            }
            cout << "[SYSTEM] Default admin created: admin/admin123\n";
        }
    }

    void resizeUsers() {
        userCapacity *= 2;
        UserClient** temp = new UserClient * [userCapacity];
        for (int i = 0; i < userCount; i++) temp[i] = users[i];
        delete[] users;
        users = temp;
    }

    void resizeCompanies() {
        companyCapacity *= 2;
        CompanyClient** temp = new CompanyClient * [companyCapacity];
        for (int i = 0; i < companyCount; i++) temp[i] = companies[i];
        delete[] companies;
        companies = temp;
    }

    void resizeEmployees() {
        employeeCapacity *= 2;
        BankEmployee** temp = new BankEmployee * [employeeCapacity];
        for (int i = 0; i < employeeCount; i++) temp[i] = employees[i];
        delete[] employees;
        employees = temp;
    }

    void resizeLinks() {
        linkCapacity *= 2;
        EmployeeLink* temp = new EmployeeLink[linkCapacity];
        for (int i = 0; i < linkCount; i++) temp[i] = employeeLinks[i];
        delete[] employeeLinks;
        employeeLinks = temp;
    }

    void loadAllData() {
        loadEmployees();
        loadUsers();
        loadCompanies();
        loadCards();
        loadEmployeeLinks();
        loadFraudAccounts();
        cleanupOrphanedLoans();
    }

    void loadEmployees() {
        ifstream file(FILE_ADMIN.c_str());
        if (!file) {
            cout << "[INFO] No admin file found. Creating default admin on first run.\n";
            return;
        }

        string line;
        while (getline(file, line)) {
            if (line.empty()) continue;

            int id = atoi(getfield(line, 0).c_str());
            string lid = getfield(line, 1);
            string pass = getfield(line, 2);
            string name = getfield(line, 3);

            if (employeeCount == employeeCapacity) {
                resizeEmployees();
            }

            employees[employeeCount++] = new BankEmployee(id, lid, pass, name);
        }
        file.close();
    }

    void loadUsers() {
        ifstream file(FILE_USERS.c_str());
        if (!file) return;

        string line;
        while (getline(file, line)) {
            if (line.empty()) continue;

            int id = atoi(getfield(line, 0).c_str());
            string name = getfield(line, 1);
            string addr = getfield(line, 2);
            string phone = getfield(line, 3);
            string cnic = getfield(line, 4);
            string lid = getfield(line, 5);
            string pass = getfield(line, 6);
            double bal = atof(getfield(line, 7).c_str());
            bool approved = (atoi(getfield(line, 8).c_str()) == 1);
            double estLimit = atof(getfield(line, 9).c_str());

            AccountType* accType = createAccountType(estLimit, "user");

            if (userCount == userCapacity) {
                resizeUsers();
            }

            users[userCount++] = new UserClient(id, name, addr, phone, cnic, lid, pass, bal, accType, approved);
        }
        file.close();
    }

    void loadCompanies() {
        ifstream file(FILE_COMPANIES.c_str());
        if (!file) return;

        string line;
        while (getline(file, line)) {
            if (line.empty()) continue;

            int id = atoi(getfield(line, 0).c_str());
            string name = getfield(line, 1);
            string addr = getfield(line, 2);
            string tax = getfield(line, 3);
            string lid = getfield(line, 4);
            string pass = getfield(line, 5);
            double bal = atof(getfield(line, 6).c_str());
            bool approved = (atoi(getfield(line, 7).c_str()) == 1);
            double estLimit = atof(getfield(line, 8).c_str());

            AccountType* accType = createAccountType(estLimit, "company");

            if (companyCount == companyCapacity) {
                resizeCompanies();
            }

            companies[companyCount++] = new CompanyClient(id, name, addr, tax, lid, pass, bal, accType, approved);
        }
        file.close();
    }

    void loadCards() {
        ifstream file(FILE_CARDS.c_str());
        if (!file) return;

        string line;
        while (getline(file, line)) {
            if (line.empty()) continue;

            int userId = atoi(getfield(line, 0).c_str());
            long long cardNum = atoll(getfield(line, 1).c_str());
            string pin = getfield(line, 2);

            UserClient* user = findUserById(userId);
            if (user) {
                user->addCard(new Card(cardNum, pin));
            }
        }
        file.close();
    }

    void loadEmployeeLinks() {
        ifstream file(FILE_EMPLOYEES_LINK.c_str());
        if (!file) return;

        string line;
        while (getline(file, line)) {
            if (line.empty()) continue;

            int userId = atoi(getfield(line, 0).c_str());
            int companyId = atoi(getfield(line, 1).c_str());

            if (linkCount == linkCapacity) {
                resizeLinks();
            }

            employeeLinks[linkCount].userId = userId;
            employeeLinks[linkCount].companyId = companyId;
            linkCount++;
        }
        file.close();
    }

    void loadFraudAccounts() {
        ifstream file(FILE_FRAUD.c_str());
        if (!file) return;

        string line;
        while (getline(file, line)) {
            if (line.empty()) continue;
            int userId = atoi(line.c_str());
            UserClient* user = findUserById(userId);
            if (user) {
                user->setfrozen(true);
                cout << "[SYSTEM] Loaded frozen user account: " << userId << endl;
            }
        }
        file.close();
    }

    void cleanupOrphanedLoans() {
        ifstream loanFile(FILE_LOANS.c_str());
        if (!loanFile) return;

        const int MAX_LOANS = 100;
        string loanLines[MAX_LOANS];
        int loanCount = 0;
        int cleanedCount = 0;

        string line;
        while (getline(loanFile, line) && loanCount < MAX_LOANS) {
            if (line.empty()) continue;

            int companyId = atoi(getfield(line, 0).c_str());
            CompanyClient* company = findCompanyById(companyId);

            if (!company) {
                // if Company doesn't exist
                cleanedCount++;
                continue;
            }

            loanLines[loanCount++] = line;
        }
        loanFile.close();

        if (cleanedCount > 0) {
            // Saving loans back to file
            ofstream outFile(FILE_LOANS.c_str());
            for (int i = 0; i < loanCount; i++) {
                outFile << loanLines[i] << "\n";
            }
            outFile.close();
            cout << "[SYSTEM] Cleaned " << cleanedCount << " orphaned loan requests.\n";
        }
    }

    AccountType* createAccountType(double estLimit, string clientType) {
        if (clientType == "user") {
            // Users limit
            if (estLimit > 100000) {
                return new Gold();
            }
            else {
                return new Bronze();
            }
        }
        else {
            // Companies limit
            if (estLimit > 500000) {
                return new Business();
            }
            else if (estLimit > 100000) {
                return new Gold();
            }
            else {
                return new Bronze();
            }
        }
    }

    UserClient* findUserById(int id) {
        for (int i = 0; i < userCount; i++) {
            if (users[i]->getId() == id) return users[i];
        }
        return NULL;
    }

    CompanyClient* findCompanyById(int id) {
        for (int i = 0; i < companyCount; i++) {
            if (companies[i]->getId() == id) return companies[i];
        }
        return NULL;
    }

    int getNextCompanyId() {
        int maxId = 0;
        for (int i = 0; i < companyCount; i++) {
            if (companies[i]->getId() > maxId) {
                maxId = companies[i]->getId();
            }
        }
        return maxId + 1;
    }

    int getNextUserId() {
        int maxId = 0;
        for (int i = 0; i < userCount; i++) {
            if (users[i]->getId() > maxId) {
                maxId = users[i]->getId();
            }
        }
        return maxId + 1;
    }

    // AUTHENTICATION METHODS
public:
    BankEmployee* authenticateEmployee(string loginId, string password) {
        for (int i = 0; i < employeeCount; i++) {
            if (employees[i]->getLoginId() == loginId &&
                employees[i]->checkPassword(password)) {
                return employees[i];
            }
        }
        return NULL;
    }

    UserClient* authenticateUser(string loginId, string password) {
        for (int i = 0; i < userCount; i++) {
            if (users[i]->getLoginId() == loginId &&
                users[i]->checkPassword(password)) {
                return users[i];
            }
        }
        return NULL;
    }

    CompanyClient* authenticateCompany(string loginId, string password) {
        for (int i = 0; i < companyCount; i++) {
            if (companies[i]->getLoginId() == loginId &&
                companies[i]->checkPassword(password)) {
                return companies[i];
            }
        }
        return NULL;
    }

    bool isLoginIdTaken(string loginId) {
        for (int i = 0; i < userCount; i++) {
            if (users[i]->getLoginId() == loginId) return true;
        }
        for (int i = 0; i < companyCount; i++) {
            if (companies[i]->getLoginId() == loginId) return true;
        }
        return false;
    }

    bool isCNICTaken(string cnic) {
        for (int i = 0; i < userCount; i++) {
            if (users[i]->getCNIC() == cnic) return true;
        }
        return false;
    }

    // REGISTRATION METHODS
    void registerClient() {
        clearScreen();
        cout << "\n=== USER REGISTRATION ===\n";

        string loginId;
        while (true) {
            cout << "Enter Login ID: ";
            loginId = getString();
            if (!Validator::validloginid(loginId)) {
                cout << "Invalid Login ID! Must be at least 5 alphanumeric characters.\n";
            }
            else if (isLoginIdTaken(loginId)) {
                cout << "Login ID already taken! Please choose another.\n";
            }
            else {
                break;
            }
        }

        string cnic;
        while (true) {
            cout << "Enter CNIC (13 digits): ";
            cnic = getString();
            if (!Validator::isvalidCNIC(cnic)) {
                cout << "Invalid CNIC! Must be exactly 13 digits.\n";
            }
            else if (isCNICTaken(cnic)) {
                cout << "CNIC already registered! Please use different CNIC.\n";
            }
            else {
                break;
            }
        }

        string name;
        while (true) {
            cout << "Enter Full Name: ";
            name = getString();
            if (!Validator::namevalid(name)) {
                cout << "Invalid name! Must be 3-50 characters with first and last name.\n";
            }
            else {
                break;
            }
        }

        string address;
        while (true) {
            cout << "Enter Address: ";
            address = getString();
            if (!Validator::validaddress(address)) {
                cout << "Invalid address! Must be 10-100 characters.\n";
            }
            else {
                break;
            }
        }

        string phone;
        while (true) {
            cout << "Enter Phone Number (11 digits starting with 03/04): ";
            phone = getString();
            if (!Validator::validphone(phone)) {
                cout << "Invalid phone number! Must be 11 digits starting with 03 or 04.\n";
            }
            else {
                break;
            }
        }

        string password;
        while (true) {
            cout << "Enter Password (min 8 chars, upper/lower case, special char): ";
            password = getString();
            if (!Validator::validPassword(password)) {
                cout << "Weak password! Must have 8+ chars, upper/lower case, and special character.\n";
            }
            else {
                break;
            }
        }

        displayAccounttypeInfo("user");

        double estLimit;
        while (true) {
            cout << "Enter Estimated Daily Withdrawal Limit: ";
            estLimit = getSafeDouble();

            if (estLimit < 0) {
                cout << "Limit cannot be negative!\n";
            }
            else if (estLimit > 500000) {
                cout << "Users cannot have limits above 500,000 PKR. Maximum for Gold account is 500,000 PKR.\n";
                cout << "Please enter a lower limit: ";
            }
            else {
                break;
            }
        }

        int newId = getNextUserId();
        AccountType* accType = createAccountType(estLimit, "user");

        if (userCount == userCapacity) {
            resizeUsers();
        }

        users[userCount++] = new UserClient(newId, name, address, phone, cnic,
            loginId, password, 0.0, accType, false);

        //ssaving to file with correct format
        ofstream file(FILE_USERS.c_str(), ios::app);
        if (file) {
            file << newId << "," << name << "," << address << "," << phone << ","
                << cnic << "," << loginId << "," << password << ",0.0,0,"
                << estLimit << "\n";
            file.close();
        }

        cout << "\n[SUCCESS] User registration completed!\n";
        cout << "Your account ID: " << newId << "\n";
        cout << "Account Type: "; accType->displayInfo(); cout << "\n";
        cout << "Status: PENDING (Awaiting bank approval)\n";

        cout << "\nPress Enter to continue...";
        cin.get();
        clearScreen();
    }

    void registerCompanyClient() {
        clearScreen();
        cout << "\n COMPANY REGISTRATION \n";

        string loginId;
        while (true) {
            cout << "Enter Login ID: ";
            loginId = getString();
            if (!Validator::validloginid(loginId)) {
                cout << "Invalid Login ID! Must be at least 5 alphanumeric characters.\n";
            }
            else if (isLoginIdTaken(loginId)) {
                cout << "Login ID already taken! Please choose another.\n";
            }
            else {
                break;
            }
        }

        string name, address, taxNumber, password;

        cout << "Enter Company Name: ";
        name = getString();

        cout << "Enter Company Address: ";
        address = getString();

        while (true) {
            cout << "Enter Company Tax Number: ";
            taxNumber = getString();
            if (taxNumber.empty()) {
                cout << "Tax number cannot be empty!\n";
            }
            else {
                break;
            }
        }

        while (true) {
            cout << "Enter Password (min 8 chars, upper/lower case, special char): ";
            password = getString();
            if (!Validator::validPassword(password)) {
                cout << "Weak password! Must have 8+ chars, upper/lower case, and special character.\n";
            }
            else {
                break;
            }
        }

        displayAccounttypeInfo("company");

        double estLimit;
        while (true) {
            cout << "Enter Estimated Daily Withdrawal Limit: ";
            estLimit = getSafeDouble();

            if (estLimit < 0) {
                cout << "Limit cannot be negative!\n";
            }
            else if (estLimit > 20000000) {
                cout << "Maximum limit for Business account is 20,000,000 PKR.\n";
                cout << "Please enter a lower limit: ";
            }
            else {
                break;
            }
        }

        int newId = getNextCompanyId();
        AccountType* accType = createAccountType(estLimit, "company");

        if (companyCount == companyCapacity) {
            resizeCompanies();
        }

        companies[companyCount++] = new CompanyClient(newId, name, address, taxNumber,
            loginId, password, 0.0, accType, false);

        // saving to file with correct format
        ofstream file(FILE_COMPANIES.c_str(), ios::app);
        if (file) {
            file << newId << "," << name << "," << address << "," << taxNumber << ","
                << loginId << "," << password << ",0.0,0," << estLimit << "\n";
            file.close();
        }

        cout << "\n[SUCCESS] Company registration completed!\n";
        cout << "Company ID: " << newId << "\n";
        cout << "Account Type: "; accType->displayInfo(); cout << "\n";
        cout << "Status: PENDING (Awaiting bank approval)\n";

        // removing  the automatic employee linking during registration
        //bank employee handle this during approval

        cout << "\nPress Enter to continue...";
        cin.get();
        clearScreen();
    }

    // transaction
    void logTransaction(int clientId, double amount, string type, int targetId = 0) {
        ofstream file(FILE_TRANS.c_str(), ios::app);
        if (file) {
            file << getcurrentdate() << "," << clientId << "," << formatLargeNumber(amount) << "," << type;
            if (targetId != 0) {
                file << "," << targetId;
            }
            file << "\n";
            file.close();
        }
    }

    void viewTransactionHistory(int clientId) {
        clearScreen();
        ifstream file(FILE_TRANS.c_str());
        if (!file) {
            cout << "No transaction history found.\n";
            return;
        }

        cout << "\n=== TRANSACTION HISTORY ===\n";
        string line;
        bool found = false;

        while (getline(file, line)) {
            if (line.empty()) continue;

            int transClientId = atoi(getfield(line, 1).c_str());
            if (transClientId == clientId) {
                cout << line << "\n";
                found = true;
            }
        }

        if (!found) {
            cout << "No transactions found.\n";
        }
        file.close();

        cout << "\nPress Enter to continue...";
        cin.get();
        clearScreen();
    }

    bool authenticateWithCard(UserClient* user) {
        if (!user->hasActiveCards()) {
            cout << "[ERROR] No active cards available.\n";
            return false;
        }

        cout << "\nSelect a card to use:\n";
        for (int i = 0; i < user->getCardCount(); i++) {
            Card* card = user->getCard(i);
            if (card) {
                cout << (i + 1) << ". Card: " << card->getNumber();
                if (!card->isCardActive()) cout << " [BLOCKED]";
                cout << "\n";
            }
        }

        int choice = getSafeInt() - 1;
        if (choice < 0 || choice >= user->getCardCount()) {
            cout << "Invalid selection.\n";
            return false;
        }

        Card* selectedCard = user->getCard(choice);
        if (!selectedCard || !selectedCard->isCardActive()) {
            cout << "Selected card is not active.\n";
            return false;
        }

        int attempts = 0;
        while (attempts < 3) {
            cout << "Enter PIN: ";
            string pin;
            cin >> pin;
            cin.ignore();

            if (selectedCard->verifyPIN(pin)) {
                return true;
            }

            attempts++;
            cout << "Invalid PIN. Attempts remaining: " << (3 - attempts) << "\n";
        }

        cout << "[ALERT] Too many failed PIN attempts! Card blocked and account frozen.\n";
        selectedCard->deactivate();
        user->setfrozen(true);

        // Log fraud
        ofstream fraudFile(FILE_FRAUD.c_str(), ios::app);
        if (fraudFile) {
            fraudFile << user->getId() << "\n";
            fraudFile.close();
        }

        saveUsersToFile();
        return false;
    }

    // USER SESSION
    void runUserSession(UserClient* user) {
        if (!user->isApproved()) {
            cout << "\n[ALERT] Your account is pending approval.\n";
            cout << "Please wait for bank employee to activate your account.\n";
            cout << "Press Enter to continue...";
            cin.get();
            clearScreen();
            return;
        }

        if (user->isfrozen()) {
            cout << "\n[ALERT] Your account has been frozen due to security concerns.\n";
            cout << "Please contact bank employee for assistance.\n";
            cout << "You cannot access your account until it is unfrozen by an administrator.\n";
            cout << "Press Enter to continue...";
            cin.get();
            clearScreen();
            return;
        }

        // Check if user is linked to any company
        int linkedCompanyId = -1;
        for (int i = 0; i < linkCount; i++) {
            if (employeeLinks[i].userId == user->getId()) {
                linkedCompanyId = employeeLinks[i].companyId;
                break;
            }
        }

        if (linkedCompanyId != -1) {
            cout << "\n[INFO] You are linked to Company ID: " << linkedCompanyId << "\n";
            cout << "1. Access Personal Account\n";
            cout << "2. Access Company Account\n";
            cout << "Select: ";
            int choice = getSafeInt();

            if (choice == 2) {
                CompanyClient* company = findCompanyById(linkedCompanyId);
                if (company) {
                    if (company->isfrozen()) {
                        cout << "\n[ALERT] The company account is frozen!\n";
                        cout << "Please contact bank employee for assistance.\n";
                        cout << "Press Enter to continue...";
                        cin.get();
                        clearScreen();
                        return;
                    }
                    runcompanySession(company);
                    return;
                }
                else {
                    cout << "Company not found!\n";
                }
            }
        }

        int choice;
        do {
            clearScreen();
            user->displayMenu();
            choice = getSafeInt();

            switch (choice) {
            case 1:
                depositMoney(user);
                break;
            case 2:
                withdrawMoney(user);
                break;
            case 3:
                transferMoney(user);
                break;
            case 4:
                viewTransactionHistory(user->getId());
                break;
            case 5:
                displayUserDetails(user);
                break;
            case 6:
                cout << "Logging out...\n";
                break;
            default:
                cout << "Invalid choice!\n";
                cout << "Press Enter to continue...";
                cin.get();
            }
        } while (choice != 6);
        clearScreen();
    }

    void depositMoney(Client* client) {
        clearScreen();
        cout << "=== DEPOSIT MONEY ===\n";
        cout << "Enter amount to deposit: ";
        double amount = getSafeDouble();

        if (amount <= 0) {
            cout << "Invalid amount!\n";
        }
        else {
            client->addBalance(amount);
            logTransaction(client->getId(), amount, "Deposit");

            if (client->getType() == "User") {
                saveUsersToFile();
            }
            else {
                saveCompanies();
            }

            cout << "[SUCCESS] Deposited " << formatLargeNumber(amount) << " PKR. New balance: " << formatLargeNumber(client->getBalance()) << " PKR\n";
        }

        cout << "\nPress Enter to continue...";
        cin.get();
        clearScreen();
    }

    void withdrawMoney(UserClient* user) {
        clearScreen();
        cout << "=== WITHDRAW MONEY ===\n";
        if (!authenticateWithCard(user)) {
            cout << "Press Enter to continue...";
            cin.get();
            clearScreen();
            return;
        }

        cout << "Enter amount to withdraw: ";
        double amount = getSafeDouble();

        if (amount <= 0) {
            cout << "Invalid amount!\n";
        }
        else if (user->canWithdraw(amount)) {
            user->subtractBalance(amount);
            user->addDailyWithdrawn(amount);
            logTransaction(user->getId(), amount, "Withdraw");
            saveUsersToFile();
            cout << "[SUCCESS] Withdrawn " << formatLargeNumber(amount) << " PKR. New balance: " << formatLargeNumber(user->getBalance()) << " PKR\n";
        }

        cout << "\nPress Enter to continue...";
        cin.get();
        clearScreen();
    }

    void transferMoney(UserClient* fromUser) {
        clearScreen();
        cout << "=== TRANSFER MONEY ===\n";
        if (!authenticateWithCard(fromUser)) {
            cout << "Press Enter to continue...";
            cin.get();
            clearScreen();
            return;
        }

        cout << "Enter target User ID: ";
        int targetId = getSafeInt();

        if (targetId == fromUser->getId()) {
            cout << "Cannot transfer to yourself!\n";
        }
        else {
            UserClient* targetUser = findUserById(targetId);
            if (!targetUser) {
                cout << "Target user not found!\n";
            }
            else if (targetUser->isfrozen()) {
                cout << "[ERROR] Cannot transfer to a frozen account!\n";
            }
            else {
                cout << "Enter amount to transfer: ";
                double amount = getSafeDouble();

                if (amount <= 0) {
                    cout << "Invalid amount!\n";
                }
                else if (fromUser->canWithdraw(amount)) {
                    fromUser->subtractBalance(amount);
                    fromUser->addDailyWithdrawn(amount);
                    targetUser->addBalance(amount);

                    logTransaction(fromUser->getId(), amount, "Transfer", targetId);
                    saveUsersToFile();

                    cout << "[SUCCESS] Transferred " << formatLargeNumber(amount) << " PKR to User ID " << targetId << "\n";
                }
            }
        }

        cout << "\nPress Enter to continue...";
        cin.get();
        clearScreen();
    }

    void displayUserDetails(UserClient* user) {
        clearScreen();
        cout << "\n=== ACCOUNT DETAILS ===\n";
        cout << "User ID: " << user->getId() << "\n";
        cout << "Name: " << user->getName() << "\n";
        cout << "CNIC: " << user->getCNIC() << "\n";
        cout << "Phone: " << user->getPhone() << "\n";
        cout << "Address: " << user->getAddress() << "\n";
        cout << "Account Type: "; user->getAccountType()->displayInfo(); cout << "\n";
        cout << "Balance: " << formatLargeNumber(user->getBalance()) << " PKR\n";
        cout << "Daily Withdrawn: " << formatLargeNumber(user->getDailyWithdrawn()) << " PKR\n";
        cout << "Status: " << (user->isApproved() ? "APPROVED" : "PENDING") << "\n";
        cout << "Frozen: " << (user->isfrozen() ? "YES - Contact Bank Employee" : "NO") << "\n";

        if (user->getCardCount() > 0) {
            cout << "\nCards:\n";
            for (int i = 0; i < user->getCardCount(); i++) {
                Card* card = user->getCard(i);
                if (card) {
                    cout << "  " << card->getNumber() << " - "
                        << (card->isCardActive() ? "ACTIVE" : "BLOCKED") << "\n";
                }
            }
        }

        cout << "\nPress Enter to continue...";
        cin.get();
        clearScreen();
    }


    // COMPANY SESSION

    void runcompanySession(CompanyClient* company) {
        if (!company->isApproved()) {
            cout << "\n[ALERT] Company account is pending approval.\n";
            cout << "Press Enter to continue...";
            cin.get();
            clearScreen();
            return;
        }

        if (company->isfrozen()) {
            cout << "\n[ALERT] Company account has been frozen!\n";
            cout << "Please contact bank employee for assistance.\n";
            cout << "You cannot access the company account until it is unfrozen by an administrator.\n";
            cout << "Press Enter to continue...";
            cin.get();
            clearScreen();
            return;
        }

        int choice;
        do {
            clearScreen();
            company->displayMenu();
            choice = getSafeInt();

            switch (choice) {
            case 1:
                depositMoney(company);
                break;
            case 2:
                withdrawCompanyMoney(company);
                break;
            case 3:
                transferToUser(company);
                break;
            case 4:
                requestLoan(company);
                break;
            case 5:
                viewTransactionHistory(company->getId());
                break;
            case 6:
                displayCompanyDetails(company);
                break;
            case 7:
                cout << "Logging out...\n";
                break;
            default:
                cout << "Invalid choice!\n";
                cout << "Press Enter to continue...";
                cin.get();
            }
        } while (choice != 7);
        clearScreen();
    }

    void withdrawCompanyMoney(CompanyClient* company) {
        clearScreen();
        cout << "=== WITHDRAW MONEY ===\n";
        cout << "Enter amount to withdraw: ";
        double amount = getSafeDouble();

        if (amount <= 0) {
            cout << "Invalid amount!\n";
        }
        else if (company->canWithdraw(amount)) {
            company->subtractBalance(amount);
            company->addDailyWithdrawn(amount);
            logTransaction(company->getId(), amount, "Withdraw");
            saveCompanies();
            cout << "[SUCCESS] Withdrawn " << formatLargeNumber(amount) << " PKR\n";
        }

        cout << "\nPress Enter to continue...";
        cin.get();
        clearScreen();
    }

    void transferToUser(CompanyClient* company) {
        clearScreen();
        cout << "=== TRANSFER TO USER ===\n";
        cout << "Enter target User ID: ";
        int targetId = getSafeInt();

        UserClient* targetUser = findUserById(targetId);
        if (!targetUser) {
            cout << "Target user not found!\n";
        }
        else if (targetUser->isfrozen()) {
            cout << "[ERROR] Cannot transfer to a frozen user account!\n";
        }
        else {
            cout << "Enter amount to transfer: ";
            double amount = getSafeDouble();

            if (amount <= 0) {
                cout << "Invalid amount!\n";
            }
            else if (company->canWithdraw(amount)) {
                company->subtractBalance(amount);
                company->addDailyWithdrawn(amount);
                targetUser->addBalance(amount);

                logTransaction(company->getId(), amount, "Transfer", targetId);
                saveCompanies();
                saveUsersToFile();

                cout << "[SUCCESS] Transferred " << formatLargeNumber(amount) << " PKR to User ID " << targetId << "\n";
            }
        }

        cout << "\nPress Enter to continue...";
        cin.get();
        clearScreen();
    }

    void requestLoan(CompanyClient* company) {
        clearScreen();
        cout << "=== REQUEST LOAN ===\n";
        cout << "Enter loan amount: ";
        double amount = getSafeDouble();

        if (amount <= 0) {
            cout << "Invalid amount!\n";
        }
        else {
            ofstream loanFile(FILE_LOANS.c_str(), ios::app);
            if (loanFile) {
                loanFile << company->getId() << "," << formatLargeNumber(amount) << ",PENDING," << getcurrentdate() << "\n";
                loanFile.close();
            }

            cout << "[SUCCESS] Loan request submitted for " << formatLargeNumber(amount) << " PKR\n";
            cout << "Awaiting bank employee approval.\n";
        }

        cout << "\nPress Enter to continue...";
        cin.get();
        clearScreen();
    }

    void displayCompanyDetails(CompanyClient* company) {
        clearScreen();
        cout << "\n COMPANY DETAILS \n";
        cout << "Company ID: " << company->getId() << "\n";
        cout << "Name: " << company->getName() << "\n";
        cout << "Tax Number: " << company->getTaxNumber() << "\n";
        cout << "Address: " << company->getAddress() << "\n";
        cout << "Account Type: "; company->getAccountType()->displayInfo(); cout << "\n";
        cout << "Balance: " << formatLargeNumber(company->getBalance()) << " PKR\n";
        cout << "Daily Withdrawn: " << formatLargeNumber(company->getDailyWithdrawn()) << " PKR\n";
        cout << "Status: " << (company->isApproved() ? "APPROVED" : "PENDING") << "\n";
        cout << "Frozen: " << (company->isfrozen() ? "YES - Contact Bank Employee" : "NO") << "\n";

        //  employees (Association)
        cout << "\nLinked Employees:\n";
        bool hasEmployees = false;
        for (int i = 0; i < linkCount; i++) {
            if (employeeLinks[i].companyId == company->getId()) {
                UserClient* emp = findUserById(employeeLinks[i].userId);
                if (emp) {
                    cout << "  User ID " << emp->getId() << ": " << emp->getName() << "\n";
                    hasEmployees = true;
                }
            }
        }
        if (!hasEmployees) {
            cout << "  No employees linked\n";
        }

        cout << "\nPress Enter to continue...";
        cin.get();
        clearScreen();
    }

    // EMPLOYEE SESSION
    void runEmployeeSession(BankEmployee* employee) {
        int choice;
        do {
            clearScreen();
            employee->displayMenu();
            choice = getSafeInt();

            switch (choice) {
            case 1: // View All Accounts
                viewAllAccounts();
                break;
            case 2: // Approve User Accounts
                approveUserAccounts();
                break;
            case 3: // Approve Company Accounts
                approveCompanyAccounts();
                break;
            case 4: // Manage Loans
                manageLoanRequests();
                break;
            case 5: // View Transactions
                viewAllTransactions();
                break;
            case 6: // Freeze/Unfreeze
                manageAccountFreezing();
                break;
            case 7: // Reset Daily Limits
                resetdailyLimits();
                break;
            case 8: // Logout
                cout << "Logging out...\n";
                break;
            default:
                cout << "Invalid choice!\n";
                cout << "Press Enter to continue...";
                cin.get();
            }
        } while (choice != 8);
        clearScreen();
    }

    void viewAllAccounts() {
        clearScreen();
        cout << "\n ALL USER ACCOUNTS \n";
        for (int i = 0; i < userCount; i++) {
            cout << "ID: " << users[i]->getId() << " | Name: " << users[i]->getName()
                << " | Balance: " << formatLargeNumber(users[i]->getBalance()) << " PKR"
                << " | Status: " << (users[i]->isApproved() ? "APPROVED" : "PENDING")
                << " | Frozen: " << (users[i]->isfrozen() ? "YES" : "NO") << "\n";
        }

        cout << "\n=== ALL COMPANY ACCOUNTS ===\n";
        for (int i = 0; i < companyCount; i++) {
            cout << "ID: " << companies[i]->getId() << " | Name: " << companies[i]->getName()
                << " | Balance: " << formatLargeNumber(companies[i]->getBalance()) << " PKR"
                << " | Status: " << (companies[i]->isApproved() ? "APPROVED" : "PENDING")
                << " | Frozen: " << (companies[i]->isfrozen() ? "YES" : "NO") << "\n";
        }

        cout << "\nPress Enter to continue...";
        cin.get();
        clearScreen();
    }

    void approveUserAccounts() {
        clearScreen();
        cout << "\n=== PENDING USER ACCOUNTS ===\n";
        bool foundPending = false;

        for (int i = 0; i < userCount; i++) {
            if (!users[i]->isApproved()) {
                foundPending = true;
                cout << "\nUser ID: " << users[i]->getId() << " | Name: " << users[i]->getName()
                    << " | CNIC: " << users[i]->getCNIC() << "\n";

                cout << "Approve this account? (1=Yes, 0=No): ";
                int decision = getSafeInt();

                if (decision == 1) {
                    // Generate card
                    long long cardNumber = generatecardnumber();
                    cout << "Generated Card Number: " << cardNumber << "\n";

                    string pin;
                    while (true) {
                        cout << "Set 4-digit PIN for the card: ";
                        pin = getString();
                        if (pin.length() == 4 && Validator::isnumeric(pin)) {
                            break;
                        }
                        cout << "Invalid PIN! Must be 4 digits.\n";
                    }

                    // Create and add card
                    users[i]->addCard(new Card(cardNumber, pin));
                    users[i]->setApproved(true);

                    // Save card to file
                    ofstream cardFile(FILE_CARDS.c_str(), ios::app);
                    if (cardFile) {
                        cardFile << users[i]->getId() << "," << cardNumber << "," << pin << "\n";
                        cardFile.close();
                    }

                    cout << "[SUCCESS] User account approved and card issued!\n";
                }
            }
        }

        if (!foundPending) {
            cout << "No pending user accounts.\n";
        }
        else {
            saveUsersToFile();
        }

        cout << "\nPress Enter to continue...";
        cin.get();
        clearScreen();
    }

    void approveCompanyAccounts() {
        clearScreen();
        cout << "\n=== PENDING COMPANY ACCOUNTS ===\n";
        bool foundPending = false;
        bool anyApproved = false;

        for (int i = 0; i < companyCount; i++) {
            if (!companies[i]->isApproved()) {
                foundPending = true;
                cout << "\nCompany ID: " << companies[i]->getId()
                    << " | Name: " << companies[i]->getName()
                    << " | Tax: " << companies[i]->getTaxNumber() << "\n";

                cout << "Approve this company? (1=Yes, 0=No): ";
                int decision = getSafeInt();

                if (decision == 1) {
                    companies[i]->setApproved(true);
                    anyApproved = true;
                    cout << "[SUCCESS] Company account approved!\n";

                    // Ask to link employees - ONLY ONCE during approval
                    cout << "Link employees to this company? (1=Yes, 0=No): ";
                    int linkChoice = getSafeInt();

                    if (linkChoice == 1) {
                        linkEmployeesToCompany(companies[i]->getId());
                    }
                }
            }
        }

        if (!foundPending) {
            cout << "No pending company accounts.\n";
        }
        else if (anyApproved) {
            saveCompanies(); // Save only if something was approved
        }

        cout << "\nPress Enter to continue...";
        cin.get();
        clearScreen();
    }

    void linkEmployeesToCompany(int companyId) {
        clearScreen();
        cout << "=== LINK EMPLOYEES TO COMPANY ===\n";
        cout << "Enter User IDs to link (enter 0 to stop):\n";

        while (true) {
            cout << "User ID: ";
            int userId = getSafeInt();

            if (userId == 0) break;

            UserClient* user = findUserById(userId);
            if (!user) {
                cout << "User not found!\n";
                continue;
            }

            // Check if link already exists
            bool alreadyLinked = false;
            for (int i = 0; i < linkCount; i++) {
                if (employeeLinks[i].userId == userId && employeeLinks[i].companyId == companyId) {
                    alreadyLinked = true;
                    break;
                }
            }

            if (alreadyLinked) {
                cout << "User already linked to this company!\n";
                continue;
            }

            // Add new link
            if (linkCount == linkCapacity) {
                resizeLinks();
            }

            employeeLinks[linkCount].userId = userId;
            employeeLinks[linkCount].companyId = companyId;
            linkCount++;

            // Save to file
            ofstream linkFile(FILE_EMPLOYEES_LINK.c_str(), ios::app);
            if (linkFile) {
                linkFile << userId << "," << companyId << "\n";
                linkFile.close();
            }

            cout << "[SUCCESS] User " << userId << " linked to company " << companyId << "\n";
        }

        cout << "\nPress Enter to continue...";
        cin.get();
        clearScreen();
    }

    void manageLoanRequests() {
        clearScreen();
        // Create temporary array to store loan lines
        const int MAX_LOANS = 100;
        string loanLines[MAX_LOANS];
        int loanCount = 0;

        // Read all loans
        ifstream loanFile(FILE_LOANS.c_str());
        if (!loanFile) {
            cout << "No loan requests found.\n";
            cout << "Press Enter to continue...";
            cin.get();
            clearScreen();
            return;
        }

        cout << "\n PENDING LOAN REQUESTS \n";
        string line;
        bool foundPending = false;

        while (getline(loanFile, line) && loanCount < MAX_LOANS) {
            if (line.empty()) continue;
            loanLines[loanCount++] = line;

            int companyId = atoi(getfield(line, 0).c_str());
            double amount = atof(getfield(line, 1).c_str());
            string status = getfield(line, 2);

            if (status == "PENDING") {
                CompanyClient* company = findCompanyById(companyId);
                if (!company) {
                    cout << "[WARNING] Loan request for non-existent Company ID: " << companyId
                        << " - " << formatLargeNumber(amount) << " PKR\n";
                    cout << "This company does not exist. Auto-rejecting...\n";

                    // Auto-reject loans for non-existent companies
                    loanLines[loanCount - 1] = getfield(line, 0) + "," + getfield(line, 1) + ",REJECTED," + getfield(line, 3);
                    continue;
                }

                foundPending = true;
                string companyName = company->getName();

                cout << "Request: Company " << companyId << " (" << companyName
                    << ") - " << formatLargeNumber(amount) << " PKR\n";

                cout << "Action? (1=Approve, 2=Reject, 0=Skip): ";
                int action = getSafeInt();

                if (action == 1) {
                    // Update in memory
                    company->addBalance(amount);

                    // Update line
                    loanLines[loanCount - 1] = getfield(line, 0) + "," + getfield(line, 1) + ",APPROVED," + getfield(line, 3);

                    // Log transaction
                    logTransaction(companyId, amount, "Loan");

                    cout << "[SUCCESS] Loan approved!\n";
                }
                else if (action == 2) {
                    loanLines[loanCount - 1] = getfield(line, 0) + "," + getfield(line, 1) + ",REJECTED," + getfield(line, 3);
                    cout << "Loan rejected.\n";
                }
            }
        }
        loanFile.close();

        if (!foundPending) {
            cout << "No pending loan requests.\n";
        }
        else {
            // Save updated loans
            ofstream outFile(FILE_LOANS.c_str());
            for (int i = 0; i < loanCount; i++) {
                outFile << loanLines[i] << "\n";
            }
            outFile.close();

            saveCompanies();
        }

        cout << "\nPress Enter to continue...";
        cin.get();
        clearScreen();
    }

    void viewAllTransactions() {
        clearScreen();
        cout << "\nEnter Client ID to view transactions (0 for all): ";
        int clientId = getSafeInt();

        ifstream file(FILE_TRANS.c_str());
        if (!file) {
            cout << "No transactions found.\n";
            cout << "Press Enter to continue...";
            cin.get();
            clearScreen();
            return;
        }

        cout << "\n=== TRANSACTIONS ===\n";
        string line;
        bool found = false;

        while (getline(file, line)) {
            if (line.empty()) continue;

            if (clientId == 0) {
                cout << line << "\n";
                found = true;
            }
            else {
                int transClientId = atoi(getfield(line, 1).c_str());
                if (transClientId == clientId) {
                    cout << line << "\n";
                    found = true;
                }
            }
        }

        if (!found) {
            cout << "No transactions found.\n";
        }
        file.close();

        cout << "\nPress Enter to continue...";
        cin.get();
        clearScreen();
    }

    void manageAccountFreezing() {
        clearScreen();
        cout << "\n=== ACCOUNT FREEZE/UNFREEZE MANAGEMENT ===\n";
        cout << "1. Freeze User Account\n";
        cout << "2. Unfreeze User Account\n";
        cout << "3. Freeze Company Account\n";
        cout << "4. Unfreeze Company Account\n";
        cout << "Select option: ";
        int choice = getSafeInt();

        cout << "Enter Client ID: ";
        int clientId = getSafeInt();

        if (choice == 1 || choice == 2) {
            // User account operations
            UserClient* user = findUserById(clientId);
            if (!user) {
                cout << "User not found!\n";
            }
            else if (choice == 1) {
                if (!user->isfrozen()) {
                    user->setfrozen(true);
                    // Add to fraud file
                    ofstream fraudFile(FILE_FRAUD.c_str(), ios::app);
                    if (fraudFile) {
                        fraudFile << clientId << "\n";
                        fraudFile.close();
                    }
                    saveUsersToFile();
                    cout << "[SUCCESS] User account frozen!\n";
                    cout << "User will not be able to login until account is unfrozen.\n";
                }
                else {
                    cout << "User account already frozen!\n";
                }
            }
            else if (choice == 2) {
                if (user->isfrozen()) {
                    user->setfrozen(false);
                    // Remove from fraud file
                    ifstream inFile(FILE_FRAUD.c_str());
                    ofstream outFile("temp_fraud.txt");
                    string line;
                    while (getline(inFile, line)) {
                        if (line.empty()) continue;
                        int id = atoi(line.c_str());
                        if (id != clientId) {
                            outFile << id << "\n";
                        }
                    }
                    inFile.close();
                    outFile.close();
                    remove(FILE_FRAUD.c_str());
                    rename("temp_fraud.txt", FILE_FRAUD.c_str());
                    saveUsersToFile();
                    cout << "[SUCCESS] User account unfrozen!\n";
                    cout << "User can now login and access their account.\n";
                }
                else {
                    cout << "User account not frozen!\n";
                }
            }
        }
        else if (choice == 3 || choice == 4) {
            // Company account operations
            CompanyClient* company = findCompanyById(clientId);
            if (!company) {
                cout << "Company not found!\n";
            }
            else if (choice == 3) {
                if (!company->isfrozen()) {
                    company->setfrozen(true);
                    saveCompanies();
                    cout << "[SUCCESS] Company account frozen!\n";
                    cout << "Company employees will not be able to access the company account until unfrozen.\n";
                }
                else {
                    cout << "Company account already frozen!\n";
                }
            }
            else if (choice == 4) {
                if (company->isfrozen()) {
                    company->setfrozen(false);
                    saveCompanies();
                    cout << "[SUCCESS] Company account unfrozen!\n";
                    cout << "Company employees can now access the company account.\n";
                }
                else {
                    cout << "Company account not frozen!\n";
                }
            }
        }
        else {
            cout << "Invalid choice!\n";
        }

        cout << "\nPress Enter to continue...";
        cin.get();
        clearScreen();
    }

    void resetdailyLimits() {
        clearScreen();
        cout << "=== RESET DAILY LIMITS ===\n";
        int resetCount = 0;

        for (int i = 0; i < userCount; i++) {
            users[i]->resetDailyLimit();
            resetCount++;
        }

        for (int i = 0; i < companyCount; i++) {
            companies[i]->resetDailyLimit();
            resetCount++;
        }

        cout << "[SUCCESS] Daily limits reset for " << resetCount << " accounts.\n";
        cout << "Press Enter to continue...";
        cin.get();
        clearScreen();
    }


    // file saving

    void saveUsersToFile() {
        ofstream file(FILE_USERS.c_str());
        if (file) {
            for (int i = 0; i < userCount; i++) {
                file << users[i]->getId() << "," << users[i]->getName() << ","
                    << users[i]->getAddress() << "," << users[i]->getPhone() << ","
                    << users[i]->getCNIC() << "," << users[i]->getLoginId() << ","
                    << users[i]->getPassword() << "," << formatLargeNumber(users[i]->getBalance()) << ","
                    << (users[i]->isApproved() ? 1 : 0) << ","
                    << formatLargeNumber(users[i]->getAccountType()->getDailyLimit()) << "\n";
            }
            file.close();
        }
    }

    void saveCompanies() {
        ofstream file(FILE_COMPANIES.c_str());
        if (file) {
            for (int i = 0; i < companyCount; i++) {
                file << companies[i]->getId() << "," << companies[i]->getName() << ","
                    << companies[i]->getAddress() << "," << companies[i]->getTaxNumber() << ","
                    << companies[i]->getLoginId() << "," << companies[i]->getPassword() << ","
                    << formatLargeNumber(companies[i]->getBalance()) << ","
                    << (companies[i]->isApproved() ? 1 : 0) << ","
                    << formatLargeNumber(companies[i]->getAccountType()->getDailyLimit()) << "\n";
            }
            file.close();
        }
    }

    // menu
    void start() {
        srand((unsigned int)time(0));

        while (true) {
            clearScreen();

            cout << "      ABC BANK MANAGEMENT SYSTEM\n";


            cout << "\nMAIN MENU \n";
            cout << "1. Login\n";
            cout << "2. Register as User\n";
            cout << "3. Register as Company\n";
            cout << "4. Exit\n";
            cout << "Select option: ";

            int choice = getSafeInt();

            switch (choice) {
            case 1:
                login();
                break;
            case 2:
                registerClient();
                break;
            case 3:
                registerCompanyClient();
                break;
            case 4:
                cout << "Thank you for using ABC Bank System. Goodbye!\n";
                return;
            default:
                cout << "Invalid choice! Please try again.\n";
                cout << "Press Enter to continue...";
                cin.get();
            }
        }
    }

    void login() {
        clearScreen();
        cout << "\n=== LOGIN ===\n";
        cout << "Login ID: ";
        string loginId = getString();

        cout << "Password: ";
        string password = getString();

        //  employee first
        BankEmployee* employee = authenticateEmployee(loginId, password);
        if (employee) {
            cout << "\n[SUCCESS] Welcome, " << employee->getName() << "!\n";
            cout << "Press Enter to continue.............";
            cin.get();
            runEmployeeSession(employee);
            return;
        }

        //  user
        UserClient* user = authenticateUser(loginId, password);
        if (user) {
            if (user->isfrozen()) {
                cout << "\n[ERROR] Your account has been frozen!\n";
                cout << "Please contact bank employee for assistance.\n";
                cout << "You cannot access your account until it is unfrozen by an administrator.\n";
                cout << "Press Enter to continue...";
                cin.get();
                clearScreen();
                return;
            }
            cout << "\n[SUCCESS] Welcome, " << user->getName() << "!\n";
            cout << "Press Enter to continue...";
            cin.get();
            runUserSession(user);
            return;
        }

        //  company
        CompanyClient* company = authenticateCompany(loginId, password);
        if (company) {
            if (company->isfrozen()) {
                cout << "\n[ERROR] Company account has been frozen!\n";
                cout << "Please contact bank employee for assistance.\n";
                cout << "You cannot access the company account until it is unfrozen by an administrator.\n";
                cout << "Press Enter to continue...";
                cin.get();
                clearScreen();
                return;
            }
            cout << "\n[SUCCESS] Welcome, " << company->getName() << "!\n";
            cout << "Press Enter to continue...";
            cin.get();
            runcompanySession(company);
            return;
        }

        cout << "\n[ERROR] Invalid login credentials!\n";
        cout << "Press Enter to continue...";
        cin.get();
        clearScreen();
    }
};

// MAIN FUNCTION
int main() {
    BankSystem bankSystem;
    bankSystem.start();
    return 0;
}