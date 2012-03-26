#include <cstdlib>
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <limits>
#include "NodeList.h"

using namespace std;

// utility class
class Reader {
public:
    Reader() : isr(&cin), lineLimit(-1), lineCount(0), eof(false) { }

    void setInputStream(istream* is) {
	isr = is;
    }

    void setLineLimit(int limit) {
	lineLimit = limit;
    }

    NodeList<int>* buildList() {
	const int QUIT_VALUE(-1000);
	const int EOL_VALUE(1000);

	NodeList<int>* data = new NodeList<int>();

	if (eof || (lineLimit != -1 && lineCount>=lineLimit)) {
	    throw domain_error("");
	}

	cout << "Enter a list..." << endl;
	int value;
	int last(-1000);
	(*isr) >> value;
	while (true) {

	    lineCount++;

	    if ((*isr).eof()) {
		// must be EOF
		if (isr != &cin) {
		    // switch from file to cin
		    isr = &cin;
		    continue;
		} else {
		    cout << "Unexpected end of input reached." << endl;
		    throw domain_error("");
		}
	    }

	    if (!(*isr)) {
		// non-numeric input
		(*isr).clear();	
		(*isr).ignore(numeric_limits<streamsize>::max(),'\n');
		cout << "error on input line " << lineCount << ". Non-numeric data ignored." << endl;
	    } else if (value < -999 || value > 999) {
		if (value == QUIT_VALUE) {
		    eof = true;
		    break;
		}

		if (value==EOL_VALUE)
		    break;

		// invalid
		cout << "error on input line " << lineCount << ". Invalid value " << value << " ignored." << endl;
	    } else {
		if (value < last) {
		    // decreasing
		    cout << "error on input line " << lineCount << ". Decreasing value " << value << " ignored." << endl;
		} else {
		    // valid
		    last = value;
		    data->push_back(value);
		}
	    }

	    if (lineLimit != -1 && lineCount>=lineLimit) {
		break;
	    }

	    (*isr) >> value;
	}

	if (lineLimit != -1 && lineCount>=lineLimit) {
	    cout << "ListReader reached the limit of "<< lineLimit << " lines of input." << endl;
	}

	return data;
    }

private:
    istream* isr;
    int lineLimit;
    int lineCount;
    bool eof;
};


void print(const NodeList<int>& data) {
    NodeList<int>::const_iterator iter;
    for (iter = data.begin(); iter != data.end(); ++iter)
	cout << *iter << " ";
    cout << endl;
}



int main(int argc, const char* argv[]) {
    filebuf fb;
    Reader reader;

    // override input based upon file, if valid
    if (argc>1) {
	fb.open(argv[1],ios::in);
	if (fb.is_open()) {
	    reader.setInputStream(new istream(&fb));
	} else {
	    cout << "Input file '" << argv[1] << "' not found." << endl;
	    cout << "Defaulting to keyboard input." << endl << endl;
	}
    }


    // is there a limit on the number of lines to read?
    int LIMIT = -1;
    if (argc>2) {
	LIMIT = atoi(argv[2]);
	if (LIMIT < 1) {
	    LIMIT = -1;
	    cout << "Second argument (" << argv[2] << ") ignored." << endl << endl;
	} else {
	    reader.setLineLimit(LIMIT);
	}
    }


    try {
	while (true) {
	    NodeList<int>* A = reader.buildList();
	    NodeList<int>* B = reader.buildList();

	    cout << endl << "Before the merge, the first list has length " << A->size() << " with contents:" << endl;
	    print(*A);
	    cout << "Before the merge, the second list has length " << B->size() << " with contents:" << endl;
	    print(*B);

#ifdef VISUAL
	    NodeList<int>::visualizer viz("merge", true);
	    viz.addList(A, "first");
	    viz.addList(B, "second");
	    viz.refresh();
#endif

	    typedef list<pair<NodeList<int>::iterator, int> > Checker;
	    Checker iteratorCheck;
	    NodeList<int>::iterator it;
	    for (it = A->begin(); it != A->end(); ++it)
		iteratorCheck.push_back(make_pair(it,*it));
	    for (it = B->begin(); it != B->end(); ++it)
		iteratorCheck.push_back(make_pair(it,*it));

	    cout << endl << "Calling merge..." << endl;
	    A->merge(*B);

#ifdef VISUAL
	    viz.refresh();
#endif

	    cout << endl << "After the merge, the first list has length " << A->size() << " with contents:" << endl;
	    print(*A);
	    cout << "After the merge, the second list has length " << B->size() << " with contents:" << endl;
	    print(*B);


	    cout << "About to check validity of all previous iterators..." << endl;
	    bool good(true);
	    for (Checker::iterator i = iteratorCheck.begin(); i != iteratorCheck.end(); ++i) {
		if (*(i->first) != i->second) {
		    good = false;
		    cout << "  ==> iterator for " << i->second << " failed" << endl;
		}
		A->erase(i->first);
	    }
	    if (A->empty() && good)
		cout << "  ==> iterators verified" << endl;

	    cout << endl;

	    delete A;
	    delete B;
	}
    } catch (domain_error) {
	// The list builder will throw an exception when '-1000' is reached in input
	cout << "GoodBye" << endl;
    }

}
