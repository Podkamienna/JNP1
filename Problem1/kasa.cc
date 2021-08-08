#include <iostream>
#include <vector>
#include <regex>
#include <unordered_map>

using namespace std;

using minutes_t = int16_t; // 0:00 - 29:59 | 0 - 9 999
static const minutes_t INVALID_MINUTES = -1;
using route_id_t = int32_t; // 0 - 999 999 999
using cost_t = int64_t; // 0 - 999 999 999
static const cost_t INVALID_MAX_COST = 1e18;
using stop_name_t = string; // ([a-zA-Z\^_]+)
using ticket_name_t = string; // ([a-zA-Z ]+)
using line_t = string; // ([^\n]*)

const minutes_t MIN_ARRIVAL_TIME = 5 * 60 + 55; ///< Maximal allowed arrival hour for trams.
const minutes_t MAX_ARRIVAL_TIME = 21 * 60 + 21; ///< Minimal allowed arrival hour for trams.
const minutes_t MAX_MINUTES = 24 * 60 - 1; ///< Maximal number of minutes.
const size_t MAX_TICKET_COUNT = 3; ///< Max allowed tickets in one proposition.
const regex TRAM_STOP_REGEX(R"(([a-zA-Z_^]+))"); ///< Matches tram stop name.
const regex ROUTE_ID_REGEX(R"(([0]*[\d]{1,9}))"); ///< Matches tram route id.
const regex TIME_REGEX(R"(([12]?[\d]:[0-5][\d]))"); ///< Matches time.
const regex ADD_ROUTE_REGEX(R"(^([0]*[\d]{1,9})( [12]?[\d]:[0-5][\d] [a-zA-Z_^]+)+$)");
const regex ADD_TICKET_REGEX(R"(^([a-zA-Z ]+) ([\d]{1,7}[.][\d]{2}) ([1-9][\d]{0,3})$)");
const regex CHOOSE_TICKETS_REGEX(R"(^[?]( [a-zA-Z_^]+ [0]*[\d]{1,9})+ ([a-zA-Z_^])+$)");

/// Structure mapping route_id and stop_name to arrival time of a tram
unordered_map<route_id_t, unordered_map<stop_name_t, minutes_t >> routeStopMap;
unordered_map<ticket_name_t, pair<cost_t, minutes_t>> tramTicketMap; ///< Structure containing all added tickets
size_t offeredTicketsCount = 0; ///< Number of the tickets offered

/*** Function declarations ***/

minutes_t whenTramStops(route_id_t id, const stop_name_t &name);

bool addRoute(route_id_t id, vector<pair<minutes_t, stop_name_t>> &stops);

bool addTicket(ticket_name_t &name, cost_t cost, minutes_t validTime);

cost_t chooseTicketsRecursively(minutes_t totalTime, size_t ticketCount, vector<ticket_name_t> &ticketNames);

bool chooseTickets(minutes_t totalTime);

bool chooseTickets(vector<stop_name_t> &stops, vector<route_id_t> &routeIds);

minutes_t stringToMinutes(const string &time);

minutes_t stringToCost(const string &cost);

bool callAddRoute(const line_t &line);

bool callAddTicket(const line_t &line);

bool callChooseTickets(const line_t &line);

bool parseAndExecuteLine(const line_t &line);

void printErrorMessage(size_t lineNumber, const line_t &line);


/*** Function definitions ***/

/**
 * @brief Finds the moment when a tram of the given route
 * stops on the given tram stop.
 * @param id — tram route number
 * @param name — name of the tram stop
 * @return Value @ref INVALID_MINUTES if there is no route with the given id or
 * there is no bus stop of the given name on that route.
 * The moment when the tram stops on the given tram stop in
 * minutes since the beginning of the day otherwise.
 */
minutes_t whenTramStops(route_id_t id, const stop_name_t &name) {
    if (routeStopMap.find(id) == routeStopMap.end()) {
        return INVALID_MINUTES;
    }

    auto &stopsOnRoute = routeStopMap[id];
    if (stopsOnRoute.find(name) == stopsOnRoute.end()) {
        return INVALID_MINUTES;
    }

    return stopsOnRoute[name];
}

/**
 * @brief Inserts new tram route to routeStopMap.
 * @param id — tram route number
 * @param stops — vector of pairs - one pair is the time, when the bus
 * arrives to the tram stop and the name of this tram stop
 * @return Value @p false if one of the times in stops is not between
 * @ref MIN_ARRIVAL_TIME and @ref MAX_ARRIVAL_TIME, the times are not ascending
 * or one of tram stops occurs twice. Value @p true otherwise.
 */
bool addRoute(route_id_t id, vector<pair<minutes_t, stop_name_t>> &stops) {
    if (stops.empty() || routeStopMap.find(id) != routeStopMap.end()) {
        return false;
    }

    unordered_map<stop_name_t, minutes_t> stopMap;
    minutes_t lastTime = 0;
    for (auto &stop : stops) {
        minutes_t time = stop.first;
        stop_name_t &name = stop.second;
        if (time <= lastTime || time < MIN_ARRIVAL_TIME || time > MAX_ARRIVAL_TIME) {
            return false;
        }
        if (stopMap.find(name) != stopMap.end()) {
            return false;
        }
        stopMap[name] = time;
        lastTime = time;
    }

    routeStopMap[id] = stopMap;
    return true;
}

/**
 * @brief Adds ticket to tramTicketVector.
 * @param name — the name of the ticket
 * @param cost — the cost of the ticket
 * @param validTime — the time span for which
 * the ticket is valid
 * @return Value @p false if the validTime is not positive or ticket already exists.
 * Value @p true otherwise.
 */
bool addTicket(ticket_name_t &name, cost_t cost, minutes_t validTime) {
    if (validTime <= 0) {
        return false;
    }

    if (tramTicketMap.find(name) != tramTicketMap.end()) {
        return false;
    }

    tramTicketMap.emplace(name, make_pair(cost, validTime));
    return true;
}

/**
 * @brief Chooses @p ticketCount tickets which validity time spans sum up to at least
 * totalTime. Has time complexity o(n^ticketCount). Appends names of found tickets to @p ticketNames.
 * @param totalTime — the time the chosen tickets must be valid through
 * @param ticketCount — the number of tickets to be chosen
 * @param ticketNames — reference to the vector of the tickets names to be populated
 * @return Value @ref INVALID_MAX_COST if there is no possibility of choosing tickets.
 * Returns the cost of the chosen tickets otherwise.
 */
cost_t chooseTicketsRecursively(minutes_t totalTime, size_t ticketCount, vector<ticket_name_t> &ticketNames) {
    if (totalTime <= 0) {
        return 0;
    }

    if (ticketCount <= 0) {
        // No tickets left and positive time to cover.
        return INVALID_MAX_COST;
    }

    // Best found cost of tickets.
    cost_t bestCost = INVALID_MAX_COST;
    // Tickets used in the cheapest combination of tickets found so far.
    vector<ticket_name_t> bestTickets;
    // Tickets used in currently checked combination.
    vector<ticket_name_t> tmpTickets;

    for (auto &ticket : tramTicketMap) {
        const ticket_name_t &name = ticket.first;
        cost_t ticketCost = ticket.second.first;
        minutes_t ticketTime = ticket.second.second;

        // Check combination where first ticket is as given.
        tmpTickets.clear();
        // Cost of the best combination.
        cost_t subResult = 0;

        if (ticketCount > 1) {
            // If more than @p 1 ticket left choose rest recursively.
            subResult = chooseTicketsRecursively(totalTime - ticketTime, ticketCount - 1, tmpTickets);
            if (subResult == INVALID_MAX_COST) {
                continue;
            }
        } else {
            // If this is the only ticket left check if it is long enough.
            if (ticketTime < totalTime) {
                continue;
            }
        }

        // Calculate complete cost and update result if needed.
        subResult += ticketCost;
        if (subResult < bestCost) {
            tmpTickets.push_back(name);
            bestCost = subResult;
            bestTickets = tmpTickets;
        }
    }

    if (bestCost == INVALID_MAX_COST) {
        return bestCost;
    }

    // Populate ticketNames with results.
    ticketNames.insert(ticketNames.end(), bestTickets.begin(), bestTickets.end());
    return bestCost;
}

/**
 * @brief Prints the result of choosing tickets to standard output.
 * @param totalTime — the time through which the chosen tickets are
 * supposed to be valid.
 * @return Value @p true
 */
bool chooseTickets(minutes_t totalTime) {
    vector<ticket_name_t> result;
    cost_t cost = chooseTicketsRecursively(totalTime, MAX_TICKET_COUNT, result);

    if (cost == INVALID_MAX_COST) {
        cout << ":-|" << endl;

        return true;
    }

    offeredTicketsCount += result.size();
    cout << "! " << result[0];
    for (size_t i = 1; i < result.size(); i++) {
        cout << "; " << result[i];
    }
    cout << endl;
    return true;
}

/**
 * @brief Calculates The time which the tickets to be chosen need to be valid for.
 * @param stops — The tram stops that will be visited.
 * @param routeIds — The ids of the routes that will be used.
 * @return Value @p false if function arguments have wrong values, value @p true otherwise
 */
bool chooseTickets(vector<stop_name_t> &stops, vector<route_id_t> &routeIds) {
    if (stops.size() != routeIds.size() + 1) {
        return false;
    }

    for (auto x : tramTicketMap) {
        cout << "TICKET " << x.first << " " << x.second.first << " " << x.second.second << endl;
    }

    minutes_t startTime = whenTramStops(routeIds[0], stops[0]);
    if (startTime == INVALID_MINUTES) {
        return false;
    }

    minutes_t previousTime = startTime;
    for (size_t i = 0; i < routeIds.size(); i++) {
        stop_name_t &currentStop = stops[i];
        stop_name_t &nextStop = stops[i + 1];
        route_id_t routeId = routeIds[i];
        minutes_t departureTime = whenTramStops(routeId, currentStop);
        minutes_t arriveTime = whenTramStops(routeId, nextStop);
        if (previousTime < departureTime) {
            cout << ":-( " << currentStop << endl;
            return true;
        }
        if (arriveTime == INVALID_MINUTES || departureTime == INVALID_MINUTES ||
            previousTime != departureTime || departureTime >= arriveTime) {
            return false;
        }
        previousTime = arriveTime;
    }

    minutes_t totalTime = previousTime - startTime + 1;

    return chooseTickets(totalTime);
}

/**
 * @brief Converts string to hour in minutes since the beginning of the day.
 * @param time — string matching regex @ref TIME_REGEX
 * @return Value @ref INVALID_MINUTES if calculated time since the beginning
 * of the day is bigger than 23 hours and 59 minutes. Number of minutes
 * since the beginning of the day otherwise.
 */
minutes_t stringToMinutes(const string &time) {
    minutes_t result = 0;

    stringstream timeStream(time);

    int hour, minutes;

    timeStream >> hour;
    timeStream.get();
    timeStream >> minutes;

    result = hour * 60 + minutes;

    if (result > MAX_MINUTES) {
        return INVALID_MINUTES;
    }

    return result;
}

/**
 * @brief Converts string to cost.
 * @param cost — string matching regex "^[0-9]{1,7}[.][0-9]{2}$"
 * @return The result of conversion.
 */
minutes_t stringToCost(const string &cost) {
    int result = 0;

    stringstream costStream(cost);

    int integerPart, fractionalPart;

    costStream >> integerPart;
    costStream.get();
    costStream >> fractionalPart;

    result = integerPart * 100 + fractionalPart;
    result = result * 1000000000;

    return result;
}

/**
 * @brief Given line of input calls function addRoute.
 * @param line — line of input matching regex @ref ADD_ROUTE_REGEX
 * @return Value @p false if argument has incorrect value, value @p true otherwise.
 */
bool callAddRoute(const line_t &line) {
    sregex_iterator regexIteratorEnd;
    sregex_iterator regexIteratorLine(line.begin(), line.end(), ROUTE_ID_REGEX);

    stringstream tramRouteStream(regexIteratorLine->str());
    route_id_t routeId;
    tramRouteStream >> routeId;

    sregex_iterator regexIteratorTime(line.begin(), line.end(), TIME_REGEX);
    sregex_iterator regexIteratorTramStop(line.begin(), line.end(), TRAM_STOP_REGEX);
    vector<pair<minutes_t, stop_name_t>> stops;

    while (regexIteratorTime != regexIteratorEnd && regexIteratorTramStop != regexIteratorEnd) {
        string time = regexIteratorTime->str();
        minutes_t minutes = stringToMinutes(time);

        if (minutes == INVALID_MINUTES) {
            return false;
        }

        ++regexIteratorTime;

        stops.emplace_back(minutes, regexIteratorTramStop->str());
        ++regexIteratorTramStop;
    }

    return addRoute(routeId, stops);
}

/**
 * @brief Given line of input calls function addTicket.
 * @param line — line of input matching regex @ref ADD_TICKET_REGEX
 * @return Value @p false if argument has incorrect value, value @p true otherwise.
 */
bool callAddTicket(const line_t &line) {
    sregex_token_iterator regexIteratorEnd;
    sregex_token_iterator regexIteratorLine(line.begin(), line.end(), ADD_TICKET_REGEX, {1, 2, 3});

    ticket_name_t ticketName = regexIteratorLine->str();
    ++regexIteratorLine;

    string cost = regexIteratorLine->str();
    cost_t ticketCost = stringToCost(cost);
    ++regexIteratorLine;

    stringstream ticketValidityMinutes(regexIteratorLine->str());
    minutes_t ticketMinutes;
    ticketValidityMinutes >> ticketMinutes;
    ++regexIteratorLine;

    return addTicket(ticketName, ticketCost, ticketMinutes);
}

/**
 * @brief Given line of input calls function chooseTickets.
 * @param line — line of input matching regex @ref CHOOSE_TICKETS_REGEX
 * @return Value @p false if argument has incorrect value, value @p true otherwise.
 */
bool callChooseTickets(const line_t &line) {
    sregex_iterator regexIteratorEnd;

    sregex_iterator regexIteratorRouteId(line.begin(), line.end(), ROUTE_ID_REGEX);
    vector<route_id_t> routeIds;
    while (regexIteratorRouteId != regexIteratorEnd) {
        stringstream routeIdStream(regexIteratorRouteId->str());
        route_id_t route;
        routeIdStream >> route;

        routeIds.push_back(route);
        ++regexIteratorRouteId;
    }

    sregex_iterator regexIteratorTramStop(line.begin(), line.end(), TRAM_STOP_REGEX);
    vector<stop_name_t> tramStops;
    while (regexIteratorTramStop != regexIteratorEnd) {
        tramStops.push_back(regexIteratorTramStop->str());
        ++regexIteratorTramStop;
    }

    return chooseTickets(tramStops, routeIds);
}

/**
 * @brief Parses line, checks its correctness and calls appropriate function.
 * @param line — A line of input
 * @return Value @p false if the line of input is incorrect,
 * value @p true otherwise.
 */
bool parseAndExecuteLine(const line_t &line) {
    if (regex_match(line, ADD_ROUTE_REGEX)) {
        return callAddRoute(line);
    }

    if (regex_match(line, ADD_TICKET_REGEX)) {
        return callAddTicket(line);
    }

    if (regex_match(line, CHOOSE_TICKETS_REGEX)) {
        return callChooseTickets(line);
    }

    return line.empty();

}

/**
 * @brief Prints error message.
 * @param lineNumber — The number of the line that error occurred in.
 * @param line — The line that number occurred in.
 */
void printErrorMessage(size_t lineNumber, const line_t &line) {
    cerr << "Error in line " << lineNumber << ": " << line << endl;
}

int main() {
    line_t line;
    size_t lineNumber = 1;

    while (getline(cin, line)) {
        if (!parseAndExecuteLine(line)) {
            printErrorMessage(lineNumber, line);
        }

        lineNumber++;
    }

    cout << offeredTicketsCount << endl;
}
