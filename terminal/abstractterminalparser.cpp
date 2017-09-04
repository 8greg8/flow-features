/*
 * Copyright (C) 2017 Gregor Koporec <gregor.koporec@gmail.com>, University of Ljubljana
 * Copyright (C) 2017 Janez Pers <janez.pers@fe.uni-lj.si>, University of Ljubljana
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "abstractterminalparser.hpp"

using namespace std;
using namespace gk;

InvalidInputException::InvalidInputException(const string& className,
        const int lineNumber,
        const string& arg)
: Exception(className, lineNumber) {
    message += "Argument " + arg + " not specified!" + "\n";
}

AbstractTerminalParser::AbstractTerminalParser(int majorVersion, int minorVersion)
:majorVersion(majorVersion), minorVersion(minorVersion){   
}


string AbstractTerminalParser::expandName(string input) {
    if (input == "") {
        return input;
    }

    wordexp_t nameExpansion;
    wordexp(input.c_str(), &nameExpansion, 0);

    string expandedName = nameExpansion.we_wordv[0];
    wordfree(&nameExpansion);

    return expandedName;
}

vector<string> AbstractTerminalParser::expandNames(vector<string> inputs) {
    vector<string> expandedNames;
    for (auto input : inputs) {
        expandedNames.push_back(expandName(input));
    }
    return expandedNames;
}