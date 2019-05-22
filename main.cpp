
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <stack>
#include <set>
#include <cstdlib>

#include <limits.h>




using namespace std;
//declarations
void loadCustomers(string fileName);
void populateRandVals(string rfile);
int myRandom(int burst);
void simulation();
void updateGlobalStats();
void printCustomerStats();
void printFinalState();
void printCustomerAddresses();
void printVerboseOutput(int currentTime, int cid, int timeInPrevState, int previousState, int nextState, int servBurst,
                        int caBurst, int remExec, int curPrio);

class Scheduler;
class Customer;

//Vars
int currentTime =0;
int maxPrio = 4;
int randOfs = 0;
int quantum = 10000;
vector <long> randVals;
vector <Customer> customerVector;
bool verbose = false;
bool scheVerbose = false;
Scheduler * scheduler;
enum State {
    SEATED,TRANS_TO_READY, TRANS_TO_SERVE,TRANS_TO_CUSTACT,TRANS_TO_PREEMPT,TRANS_TO_DONE
};
string stateString[]= {"SEATED","READY", "SERVE","CUST-ACT","PREEMPT", "DONE"};
bool preemptAdd = false;


//globalStates
int maxfintime = 0;
int totalTurnAround = 0;
int totalServerWait = 0;
double totalServeUtil = 0;
set <int> cASet;
double totalCAUtil = 0;
double totalCustomers = 0;

struct Customer{

    int cid; //customer id
    int at; //arrival time
    int tc; // total service time for waiter
    int sb; //base service burst
    int cab; //base customer activity burst

    int state_ts = 0;
    int CT = 0; //total customer activity time
    int CW = 0; //customer wait time

    int static_prio;
    int dynamic_prio; //reset to static-1

    int execTimeRemain;  //total service time remaining
    int remainingSB; //remaining service burst



};
int lastremainingSB = 0;
int lastexecTimeRemain = 0;

struct Event {

    int evtTimeStamp;
    Customer * evtCust;
    State transition;
    State previousState;

    Event(int evtTimeStamp, Customer* evtCust, State previousState, State transition){
        this->evtTimeStamp = evtTimeStamp;
        this->evtCust=evtCust;
        this->transition = transition;
        this->previousState = previousState;
    }
    Event(){
        this->evtTimeStamp = 0;
        this->evtCust= nullptr;
        this->transition = SEATED;
        this->previousState = SEATED;
    }
};


class Scheduler{
public:
    string name = "base";
    virtual void add_customer(Customer *customer){};
    virtual Customer * get_next_customer(){Customer *cust;
        return cust;};
    virtual void add_customerPre(Customer *customer){};
    virtual void getRunQueueSize(){};
    virtual string getSchedulerName(){return "";};
};

class FCFS : public Scheduler{
public:
    string name = "FCFS";

    queue <Customer *> runQueue;

    void add_customer(Customer *customer){

        runQueue.push(customer);


    }

    Customer * get_next_customer(){
        Customer *nextCust= nullptr;
        if(!runQueue.empty()){

            nextCust = runQueue.front();
            runQueue.pop();


        }
        return nextCust;
    };
    void getRunQueueSize(){
        cout<< runQueue.size()<<endl;
    }
    string getSchedulerName(){
        return name;
    }
};

class LCFS : public Scheduler{
public:
    string name = "LCFS";
    stack <Customer *> runQueue;

    void add_customer(Customer * cust){
        runQueue.push(cust);
    }

    Customer * get_next_customer(){
        Customer *nextCust= nullptr;
        if(!runQueue.empty()){
            nextCust = runQueue.top();
            runQueue.pop();
        }
        return nextCust;
    }
    void getRunQueueSize(){
        cout<< runQueue.size()<<endl;
    }
    string getSchedulerName(){
        return name;
    }

};

class SRTF : public Scheduler{
public:
    string name = "SRTF";

    vector <Customer *> runQueue;

    void add_customer(Customer *cust){
        runQueue.push_back(cust);
        idxOfShortest = findShortest();

    }

    Customer * get_next_customer(){
        Customer *nextCust= nullptr;
        idxOfShortest = findShortest();
        if(!runQueue.empty()){

            nextCust = runQueue.at(idxOfShortest);
            runQueue.erase(runQueue.begin()+idxOfShortest);
        }
        return nextCust;
    }

    void getRunQueueSize(){
        cout<< runQueue.size()<<endl;
    }

    string getSchedulerName(){
        return name;
    }

private:
    int findShortest(){
        int i = 0;
        int index = -1;
        int shortestTime = INT_MAX;
        //cout<<runQueue.size()<<endl;
        if(!runQueue.empty()){
            while(i < runQueue.size()){
                int remain = runQueue.at(i)->execTimeRemain;
                //cout<<"index: "<<i<< " time: "<< remain<<endl;

                if(remain < shortestTime){
                    index = i;
                    shortestTime = remain;
                }
                i++;
            }
        }
        return index;
    }

    int idxOfShortest = -1;

};

class RR : public Scheduler{
public:
    string name = "RR";

    queue <Customer *> runQueue;
    //pretty much the same as FCFS
    void add_customer(Customer *cust){
        runQueue.push(cust);

    }

    Customer * get_next_customer(){
        Customer *nextCust= nullptr;
        if(!runQueue.empty()){

            nextCust = runQueue.front();
            runQueue.pop();


        }
        return nextCust;
    };
    void getRunQueueSize(){
        cout<< runQueue.size()<<endl;
    }
    string getSchedulerName(){
        return name;
    }
};

class PRIO : public Scheduler{
public:
    string name = "PRIO";

    void add_customerPre(Customer * cust){

        int currentPrio = cust->dynamic_prio;

        if(currentPrio>0){ //lower the prio quantum ended.
            currentPrio -= 1;
            cust->dynamic_prio = cust->dynamic_prio -1;
            activeQ->at(currentPrio).push(cust);
        } else { //add to expired queue
            //reset to static
            cust->dynamic_prio = cust->static_prio-1;
            expiredQ->at(cust->dynamic_prio).push(cust);
        }

    }

    void add_customer(Customer *cust){
        int currentPrio = cust->dynamic_prio;

        activeQ->at(currentPrio).push(cust);




    }
    Customer * get_next_customer(){
        //check if activeQ is empty, if it is, swap.

        if(isActQEmtpy()){
            if(activeQ==&q1){
                activeQ = &q0;
                expiredQ = &q1;
            } else{
                activeQ = &q1;
                expiredQ = &q0;
            }
        }

        Customer *cust = nullptr;

        //find the next cust in an activeQ
        for(int i = activeQ->size()-1; i >=0 ; i--){

            if(activeQ->at(i).size()>0){
                cust = activeQ->at(i).front();
                activeQ->at(i).pop();
                break;
            }

        }

        return cust;
    }

    bool isActQEmtpy(){

        bool empty = true;
        for(int i = 0; i < activeQ->size(); i++){
            if(!activeQ->at(i).empty()){
                empty = false;
            }

        }
        return empty;

    }

    PRIO(){


        for(int i = 0; i < maxPrio; i++){
            queue <Customer *> runQueue;
            queue <Customer *> expQueue;
            activeQ->push_back(runQueue);
            expiredQ->push_back(expQueue);

        }

        //cout<<"act queues: "<<activeQ->size()<<endl;
        //cout<<"exp queues: "<<expiredQ->size()<<endl;

    }

    string getSchedulerName(){
        return name;
    }

private:
    vector<queue<Customer *> > * activeQ = &q1;
    vector<queue<Customer *> > * expiredQ = &q0;
    vector<queue<Customer *> > q0;
    vector<queue<Customer *> > q1;

};

class PREPRIO : public Scheduler{
public:
    string name = "PREPRIO";

    void add_customerPre(Customer * cust){

        int currentPrio = cust->dynamic_prio;

        if(currentPrio>0){ //lower the prio quantum ended.
            currentPrio -= 1;
            cust->dynamic_prio = cust->dynamic_prio -1;
            activeQ->at(currentPrio).push(cust);
        } else { //add to expired queue
            //reset to static
            cust->dynamic_prio = cust->static_prio-1;
            expiredQ->at(cust->dynamic_prio).push(cust);
        }

    }

    void add_customer(Customer *cust){
        int currentPrio = cust->dynamic_prio;

        activeQ->at(currentPrio).push(cust);

    }
    Customer * get_next_customer(){
        //check if activeQ is empty, if it is, swap.

        if(isActQEmtpy()){
            if(activeQ==&q1){
                activeQ = &q0;
                expiredQ = &q1;
            } else{
                activeQ = &q1;
                expiredQ = &q0;
            }
        }

        Customer *cust = nullptr;

        //find the next cust in an activeQ
        for(int i = activeQ->size()-1; i >=0 ; i--){

            if(activeQ->at(i).size()>0){
                cust = activeQ->at(i).front();
                activeQ->at(i).pop();
                break;
            }

        }

        return cust;
    }

    bool isActQEmtpy(){

        bool empty = true;
        for(int i = 0; i < activeQ->size(); i++){
            if(!activeQ->at(i).empty()){
                empty = false;
            }

        }
        return empty;

    }

    PREPRIO(){


        for(int i = 0; i < maxPrio; i++){
            queue <Customer *> runQueue;
            queue <Customer *> expQueue;
            activeQ->push_back(runQueue);
            expiredQ->push_back(expQueue);

        }


    }

    string getSchedulerName(){
        return name;
    }

private:
    vector<queue<Customer *> > * activeQ = &q1;
    vector<queue<Customer *> > * expiredQ = &q0;
    vector<queue<Customer *> > q0;
    vector<queue<Customer *> > q1;
};




//DES LAYER FUNCTION
vector <Event> eventQueue;

Event get_event(){
    Event frontEvent;
    if(!eventQueue.empty()){
        frontEvent = eventQueue.front();
        eventQueue.erase(eventQueue.begin());

    }
    return frontEvent;
}

void put_Event(Event evt){

    int i = 0;
    int size = eventQueue.size();


    if(eventQueue.empty()){
        eventQueue.push_back(evt);
    } else {
        while(i < size){
//            cout<<"size:" <<size<<endl;
//            cout<<"event time: "<<evt.evtTimeStamp<<endl;
//            cout<<"event in queue time: "<< eventQueue.at(i).evtTimeStamp<<endl;
            if(evt.evtTimeStamp < eventQueue.at(i).evtTimeStamp){
                break;
            }
            i++;
        }
        eventQueue.insert(eventQueue.begin()+i,evt);
    }
}

int get_next_event_time(){
    return eventQueue.front().evtTimeStamp;
}

void rm_event(Event *evt){
    if(!eventQueue.empty()){
        for(int i = 0; i < eventQueue.size(); i++){
            if(evt == &eventQueue.at(i)){
                eventQueue.erase(eventQueue.begin() + i);
                break;
            }
        }

    }
}



//Simulation Function


void simulation(){


    //Event evt;
    //int currentTime;
    //int timeInPrevState;
    bool callScheduler;
    Customer * CURRENT_CUSTOMER= nullptr;


    while(!eventQueue.empty()){

        Event evt = get_event();

        Customer *cust = evt.evtCust;
        currentTime = evt.evtTimeStamp;
        int timeInPrevState = currentTime-(cust->state_ts);

        int sbEndTime = -1;

        if(CURRENT_CUSTOMER != nullptr){
            if(lastremainingSB < quantum){
                sbEndTime = CURRENT_CUSTOMER->state_ts+lastremainingSB;
            } else{
                sbEndTime = CURRENT_CUSTOMER->state_ts+quantum;
            }


        }


        switch (evt.transition) {

            //comes from BLOCKED, PREEMPTION or CREATION
            case TRANS_TO_READY:
            {
                if (verbose) {
                    printVerboseOutput(currentTime, cust->cid, timeInPrevState, evt.previousState, TRANS_TO_READY, -1, -1, -1, -1);
                }

                if(verbose && scheduler->getSchedulerName() == "PREPRIO"&& CURRENT_CUSTOMER != nullptr){
                    int runningCustPrio = CURRENT_CUSTOMER->dynamic_prio;
                    int incomingCustPrio = cust->dynamic_prio;

                    if ((currentTime<sbEndTime && incomingCustPrio>runningCustPrio)){
                        cout<<"---> PRIO preemption "<<CURRENT_CUSTOMER->cid<<" by "<<cust->cid<<" ? "<<(CURRENT_CUSTOMER->dynamic_prio<cust->dynamic_prio)<<" TS="<<sbEndTime<<" now="<< currentTime <<") --> YES"<<endl;
                    }
                    else{
                        cout<<"---> PRIO preemption "<<CURRENT_CUSTOMER->cid<<" by "<<cust->cid<<" ? "<< (CURRENT_CUSTOMER->dynamic_prio < cust->dynamic_prio)<<" TS="<<sbEndTime<<" now="<<currentTime<<") --> NO"<<endl;
                    }

                }


                //check if scheduler is preprio and whether the event trans-to-ready is arriving during the current customer
                if(scheduler->getSchedulerName() == "PREPRIO" && CURRENT_CUSTOMER != nullptr && currentTime<sbEndTime){

                    int runningCustPrio = CURRENT_CUSTOMER->dynamic_prio;
                    int incomingCustPrio = cust->dynamic_prio;

                    //check if the incoming customer has a higher prio

                    if(incomingCustPrio>runningCustPrio){

                        Event *removeEvt;
                        for(int i = 0; i < eventQueue.size(); i++){
                            if(eventQueue.at(i).evtCust == CURRENT_CUSTOMER){
                                removeEvt = &eventQueue.at(i);
                                rm_event(removeEvt);
                                break;
                            }
                        }
                        //update current running customer then create a new event

                        int delta = 0;
                        if(lastremainingSB < quantum){
                            delta = lastremainingSB;
                        } else{
                            delta = quantum;
                        }
                        CURRENT_CUSTOMER->remainingSB = CURRENT_CUSTOMER->remainingSB+delta-(currentTime-CURRENT_CUSTOMER->state_ts);
                        CURRENT_CUSTOMER->execTimeRemain += delta-(currentTime-CURRENT_CUSTOMER->state_ts);
                        put_Event(Event(currentTime,CURRENT_CUSTOMER, TRANS_TO_SERVE, TRANS_TO_PREEMPT));

                        CURRENT_CUSTOMER = nullptr;

                    }
                }


                cust->state_ts = currentTime;

                scheduler->add_customer(cust);
                //print transition if verbose

                callScheduler = true;
                break;
            }
            case TRANS_TO_SERVE:
            {
                //update params
                int remaingSB = cust->remainingSB;
                int remExec = cust->execTimeRemain;
                int baseBurst = cust->sb;
                int serveBurst = -1;
                cust->state_ts = currentTime;
                cust->CW += timeInPrevState;

                totalServerWait += timeInPrevState;

                //check to see if there is an unfinished burst, if not, issue new burst
                if(remaingSB>0){
                    serveBurst = remaingSB;
                }

                //get new service burst if there is none.
                if(serveBurst == -1){
                    serveBurst = myRandom(baseBurst);
                }
                //reduce service Burst to remaining service time
                if(serveBurst > remExec){
                    serveBurst = remExec;
                }

                //Decide next state
                int nextState = -1;
                if(serveBurst - quantum > 0){
                    nextState = TRANS_TO_PREEMPT;
                }else if(remExec-serveBurst > 0  && serveBurst <= quantum){//create an event to block if there is still remaining exec
                    //cout<<"block"<<endl;
                    nextState = TRANS_TO_CUSTACT;
                } else if(remExec - serveBurst <= 0) { //trans to done
                    nextState = TRANS_TO_DONE;
                }

                lastexecTimeRemain =remExec;
                lastremainingSB = serveBurst;
                //print transition
                if (verbose) {
                    printVerboseOutput(currentTime, cust->cid, timeInPrevState, evt.previousState, TRANS_TO_SERVE,
                                       serveBurst, -1, remExec, cust->dynamic_prio);
                }

                //create new event based on determined next state.
                switch (nextState){
                    case TRANS_TO_PREEMPT:{
                        if(serveBurst-quantum >=0){
                            cust->remainingSB = serveBurst-quantum;
                        } else {
                            cust->remainingSB = 0;
                        }
                        cust->execTimeRemain = remExec-quantum;
                        put_Event(Event(currentTime+quantum,cust,TRANS_TO_SERVE,TRANS_TO_PREEMPT));
                        break;
                    }
                    case TRANS_TO_CUSTACT:{
                        //decrement the total exec time remaining
                        cust->execTimeRemain= remExec-serveBurst;
                        if(remaingSB-serveBurst >=0){
                            cust->remainingSB = remaingSB-serveBurst;
                        } else {
                            cust->remainingSB = 0;
                        }


                        put_Event(Event(currentTime+serveBurst, cust, TRANS_TO_SERVE, TRANS_TO_CUSTACT));
                        break;
                    }
                    case TRANS_TO_DONE:{
                        //zero out execution time
                        cust->execTimeRemain = 0;
                        put_Event(Event(currentTime+serveBurst,cust,TRANS_TO_SERVE,TRANS_TO_DONE));
                        break;
                    }
                }

                break;
            }

            case TRANS_TO_CUSTACT:
            {
                //Update parameters
                int baseCABurst = cust->cab;
                int caBurst = myRandom(baseCABurst);
                int remExec = cust->execTimeRemain;
                CURRENT_CUSTOMER = nullptr;

                cust->state_ts = currentTime;
                cust->dynamic_prio = cust->static_prio-1;

                //update customer ca time
                cust->CT +=caBurst;

                //update global ca time
                int temp = currentTime;
                for(int i = caBurst; i > 0; i--){
                    temp++;
                    cASet.insert(temp);
                }

                // Event newEvent;
                put_Event(Event(currentTime+caBurst,cust,TRANS_TO_CUSTACT,TRANS_TO_READY));

                if (verbose) {
                    printVerboseOutput(currentTime, cust->cid, timeInPrevState, evt.previousState, TRANS_TO_CUSTACT, -1,
                                       caBurst, remExec, -1);
                }
                //create event to transition to run


                callScheduler = true;
                break;
            }

            case TRANS_TO_DONE:
            {
                if (verbose) {
                    printVerboseOutput(currentTime, cust->cid, timeInPrevState, evt.previousState, TRANS_TO_DONE, -1,
                                       -1, -1, -1);
                }
                //update end time and remove current customer.
                cust->state_ts = currentTime;
                CURRENT_CUSTOMER = nullptr;

                callScheduler = true;

                break;
            }
            case TRANS_TO_PREEMPT:
            {
                cust->state_ts = currentTime;

                CURRENT_CUSTOMER = nullptr;

                preemptAdd = true;

                if (verbose) {

                    printVerboseOutput(currentTime, cust->cid, timeInPrevState, evt.previousState, TRANS_TO_READY, cust->remainingSB, -1, cust->execTimeRemain, cust->dynamic_prio);
                }

                if(scheduler->getSchedulerName() == "PRIO" || scheduler->getSchedulerName() == "PREPRIO"){

                    scheduler->add_customerPre(cust);
                } else {
                    scheduler->add_customer(cust);
                }




                callScheduler = true;
                break;
            }
            case SEATED:
            {
                break;
            }

        }

        if(callScheduler){

            if(!eventQueue.empty() && get_next_event_time() == currentTime){
                continue;
            }
            callScheduler = false; //reset global flag


            if(CURRENT_CUSTOMER == nullptr){

                CURRENT_CUSTOMER = scheduler->get_next_customer();

                if(CURRENT_CUSTOMER == nullptr){
                    continue;
                }

                put_Event(Event(currentTime, CURRENT_CUSTOMER,TRANS_TO_READY, TRANS_TO_SERVE ));

            }

        }


    }
    //update global stats at the end of simulation
    updateGlobalStats();

}



//Helper Functions
void populateRandVals(string rfile){
    ifstream inputFile(rfile);
    int lineCounter =0;
    int totalNums = 0;
    int number;

    if(inputFile.is_open()){
        while (inputFile >> number)
        {
            if(lineCounter ==0){
                totalNums = number;
            } else {
                randVals.push_back(number);
            }
            lineCounter++;
        }
    }else{
        cout<<"Error in reading file"<<endl;
    }
    if (totalNums != randVals.size()){
        cout << "rfile mismatch. indicated count: "<< totalNums<< " parsed count:" << randVals.size()<< endl;
    }
}

int myRandom(int burst){

    if(randOfs>=randVals.size()){
        //cout<<"offset: "<<randOfs<<" resetting now"<<endl;
        randOfs = 0;  //wrap around
    }
    //cout<<"time: "<<currentTime<<" burst:"<< burst<<endl;
    int randBurst = 1+(randVals.at(randOfs)%burst);
    randOfs++;
    return randBurst;
}

//read input file
void loadCustomers(string fileName){
    ifstream inputFile(fileName);
    int arrive;
    int totalservice;
    int sburst;
    int caburst;

    if(inputFile.is_open()){
        //cout<<"opened"<<endl;
        int numCounter =0;
        while (inputFile >> arrive >> totalservice >> sburst >> caburst)
        {
            Customer newCust;
            newCust.cid = numCounter;
            newCust.at = arrive;
            newCust.tc = totalservice;
            newCust.sb = sburst;
            newCust.cab = caburst;
            newCust.state_ts = arrive;
            newCust.execTimeRemain = totalservice;
            newCust.remainingSB = -1;
            int tempPrio = myRandom(maxPrio);
            newCust.static_prio = tempPrio;
            newCust.dynamic_prio = tempPrio-1;

            customerVector.push_back(newCust);
            numCounter++;
        }
    }else{
        cout<<"Error in reading file"<<endl;
    }

    totalCustomers = customerVector.size();

    //add initial events
    for (int i = 0; i < customerVector.size(); i++) {
        put_Event(Event(customerVector.at(i).at,&customerVector.at(i), SEATED, TRANS_TO_READY));
    }
}

//update global stats
void updateGlobalStats(){
    for(int i = 0; i < customerVector.size(); i++){

        int arrival = customerVector.at(i).at;
        int totaltime = customerVector.at(i).tc;
        int state_ts = customerVector.at(i).state_ts;
        int turnaround = state_ts - arrival;

        //update global states
        if(state_ts > maxfintime){
            maxfintime = state_ts;
        }
        totalTurnAround += turnaround;
        totalServeUtil += totaltime;


    }
    totalCAUtil = cASet.size();
}

//print functions
void printCustomerStats(){
    //print each customer's statistics
    for(int i = 0; i < customerVector.size(); i++){

        int id = customerVector.at(i).cid;
        int arrival = customerVector.at(i).at;
        int totaltime = customerVector.at(i).tc;
        int servburst = customerVector.at(i).sb;
        int caburst = customerVector.at(i).cab;
        int static_prio = customerVector.at(i).static_prio;
        int state_ts = customerVector.at(i).state_ts; //customer finish time
        int cacttime = customerVector.at(i).CT; //total customer activity time
        int custwaittime = customerVector.at(i).CW; //time spent in ready state
        int turnaround = state_ts - arrival;


        printf("%04d: %4d %4d %4d %4d %1d | %5d %5d %5d %5d\n",
               id,
               arrival, totaltime, servburst, caburst, static_prio,
               state_ts, // last time stamp
               turnaround,
               cacttime,
               custwaittime);
    }
}

void printFinalState(){

    //maxfintime is global value from update function.
    double server_util = (totalServeUtil/maxfintime)*100; //average server utilization
    double ca_util  = (totalCAUtil/maxfintime)*100; //average customer activity utilization
    double avg_turnaround = totalTurnAround/totalCustomers;
    double avg_waittime = totalServerWait/totalCustomers;
    double throughput = (totalCustomers/maxfintime)*100;

    printf("SUM: %d %.2lf %.2lf %.2lf %.2lf %.3lf\n",
           maxfintime,
           server_util,
           ca_util,
           avg_turnaround,
           avg_waittime,
           throughput);

}

void printCustomerAddresses(){
    for(int i = 0; i < customerVector.size();i++){

        cout<< customerVector.at(i).cid << " " <<&customerVector.at(i)<<endl;
    }
    cout<<endl;
}

void printVerboseOutput(int currentTime, int cid, int timeInPrevState, int previousState, int nextState, int servBurst,
                        int caBurst, int remExec, int curPrio){

    switch(nextState){
        case TRANS_TO_SERVE:{
            cout << currentTime << " " << cid << " " << timeInPrevState << " : "
                 << stateString[previousState] << " -> " << stateString[nextState] << " sb=" << servBurst <<" rem="<<remExec<< " prio="<< curPrio<<endl;
            break;
        }
        case TRANS_TO_CUSTACT:{
            cout << currentTime << " " << cid << " " << timeInPrevState << " : "
                 << stateString[previousState] << " -> " << stateString[nextState] << " cab=" << caBurst <<" rem="<<remExec<<endl;
            break;
        }
        case TRANS_TO_DONE:{
            cout << currentTime << " " << cid << " " << timeInPrevState << " : "
                 << stateString[nextState] <<endl;
            break;
        }
        case TRANS_TO_READY:{
            cout << currentTime << " " << cid << " " << timeInPrevState << " : "
                 << stateString[previousState] << " -> " << stateString[nextState];
            if(servBurst>0){
                cout<<" sb="<<servBurst<<" "<< "rem="<< remExec<< " prio="<< curPrio<<endl;
            } else {
                cout<<endl;
            }
            break;
        }

    }

}

int main(int argc, char *argv[]) {

    string rfileLoc;
    string inputFile;
    string inputSch;
    //parse input args
    for(int i = 1; i < argc; i++){
        string arg = argv[i];
        size_t found = arg.find_last_of("/\\");
        string temp = arg.substr(found+1);
//        if(temp.find("input") != string::npos) { //set inputx file
//            inputFile = argv[i];
//
//        }

        if(temp.find("-v") != string::npos){//set verbose
            //cout<<"verbose"<<endl;
            verbose = true;
        }
        if(temp.find("rfile") != string::npos){ //set rfile loc
            rfileLoc = argv[i];
            //cout<<rfileLoc<<endl;
        }
        if(temp.find("input") != string::npos) { //set inputx file
            inputFile = argv[i];
            //cout<<inputFile<<endl;
        }
        if(temp.find("-s") != string::npos) { //extract scheduler
            inputSch = argv[i];
            //cout<<inputSch<<endl;
        }
        //else{
//            cout<<"Error reading arguments. Exiting now"<<endl;
//            exit(0);
//        }
    }


    scheVerbose = false;  //print out scheduler specific events

    char schedAlgo = inputSch.at(2);

    int prioIdx = 0;
    int inputLen =inputSch.length();

    if(schedAlgo == 'R' || schedAlgo == 'P' || schedAlgo == 'E'){
        int customTimeSlice = 0;
        int customPrio = 0;
        for(int i = 3; i < inputLen; i++){
            if(inputSch.at(i)==':'){
                for(int j = i+1; j < inputLen; j++){
                    customPrio = customPrio*10+(inputSch.at(j)-48);
                }
                maxPrio = customPrio;
                break;
            }

            customTimeSlice = customTimeSlice*10+(inputSch.at(i)-48);
        }
        quantum = customTimeSlice;
        // cout<<quantum<<endl;
    }

    //cout<<"max prio: "<< maxPrio<<endl;
    //cout<<"quantum: "<< quantum<<endl;

    //set scheduler algorithm
    switch(schedAlgo){
        case 'F':{
            scheduler = new FCFS();
            break;
        }
        case 'L':{
            scheduler = new LCFS();
            break;
        }
        case 'S':{
            scheduler = new SRTF();
            break;
        }
        case 'R':{
            scheduler = new RR();
            break;
        }
        case 'P':{
            //cout<<"max prio: "<<maxPrio<<endl;
            scheduler = new PRIO();
            break;
        }
        case 'E':{
            scheduler = new PREPRIO();
            break;
        }
    }


    populateRandVals(rfileLoc);  //load rFile into
    loadCustomers(inputFile);

    //run simulation

    simulation();

    //print customer and summary stats
    cout << scheduler->getSchedulerName();
    if(quantum != 10000){
        cout<<" " << quantum<<endl;
    } else {
        cout<<endl;
    }
    printCustomerStats();
    printFinalState();




    return 0;
}