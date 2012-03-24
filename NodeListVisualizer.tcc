// The code after here is for drawing a representation of the list using GraphViz.  It
// is not needed for use of the class.
#include <fstream>
#include <map>
#include <set>
#include <list>
#include <iostream>
#include "NodeList.h"
using namespace std;


template <typename Object>
class NodeList<Object>::visualizer {
private:
  string name;  // filename
  bool pause;   // should we pause after each refresh?
  typedef map<NodeList*,string> lmap;
  lmap lists;
  typedef map<typename NodeList::iterator*,string> imap;
  imap iterators;
  typedef map<typename NodeList::Node*,string> ndmap;
  ndmap nodes;


public:
  visualizer(string name, bool pause) : name(name), pause(pause) {}

  void addList(NodeList* list, string label) {
    lists[list] = label;
  }
  void removeList(NodeList* list) {
    typename lmap::iterator temp(lists.find(list));
    if (temp != lists.end())
      lists.erase(temp);
  }

  void addIterator(NodeList::iterator* iter, string label) {
    iterators[iter] = label;
  }
  void removeIterator(NodeList::iterator* iter) {
    typename imap::iterator temp(iterators.find(iter));
    if (temp != iterators.end())
      iterators.erase(temp);
  }

  void addNodePt(NodeList::Node* nd, string label) {
    nodes[nd] = label;
  }

  void removeNodePt(NodeList::Node* nd) {
    typename ndmap::iterator temp(nodes.find(nd));
    if (temp != nodes.end())
      nodes.erase(temp);
  }

  void refresh() {
    ofstream out((name + ".dot").c_str());
    out << "digraph {" << endl;
    out << "rankdir=LR;" << endl;

    // Find all of the possible nodes to consider
    set<Node *> nodeset;
    list<Node *> toExpand;

    for (typename lmap::iterator it = lists.begin(); it != lists.end(); ++it) {
      cout << "processing list " << it->second << endl;
      Node* _front = it->first->header;
      if (_front) {
	nodeset.insert(_front);
	toExpand.push_back(_front);
	out << it->second << "_header [shape=ellipse];" << endl;
      }
      Node* _back = it->first->
trailer;
      if (_back) {
	nodeset.insert(_back);
	toExpand.push_back(_back);
	out << it->second << "_trailer [shape=ellipse];" << endl;
      }
    }

    for (typename imap::iterator it = iterators.begin(); it != iterators.end(); ++it) {
      Node* _iterNode = it->first->node;
      nodeset.insert(_iterNode);
      toExpand.push_back(_iterNode);
      out << it->second << " [shape=rectangle];" << endl;
    }


    for (typename ndmap::iterator it = nodes.begin(); it != nodes.end(); ++it) {
      Node* _iterNode = it->first;
      nodeset.insert(_iterNode);
      toExpand.push_back(_iterNode);
      out << it->second << " [shape=rectangle];" << endl;
    }

    while (!toExpand.empty()) {
      Node *next = toExpand.front();
      toExpand.pop_front();
      if (next && next->next  && nodeset.find(next->next) == nodeset.end()) {
	nodeset.insert(next->next);
	toExpand.push_back(next->next);
      }
      if (next && next->prev && nodeset.find(next->prev) == nodeset.end()) {
	nodeset.insert(next->prev);
	toExpand.push_back(next->prev);
      }
    }

    for (typename set<Node *>::iterator iter = nodeset.begin(); iter != nodeset.end(); ++iter) {
      out << "node" << (long) *iter << " [shape=record, label=\"" << (*iter)->elem;
      out << " | { <prev> | <next> }\"];" << endl;
    }

    // link headers/trailers to nodes
    for (typename lmap::iterator it = lists.begin(); it != lists.end(); ++it) {
      // Links
      Node* _front = it->first->header;
      if (_front)
	out << it->second << "_header -> node" << (long) _front << ":w;" << endl;
      Node* _back = it->first->trailer;
      if (_back)
	out << "node" << (long) _back << ":e -> " << it->second << "_trailer [dir=back];" << endl;
    }

    for (typename set<Node *>::iterator iter = nodeset.begin(); iter != nodeset.end(); ++iter) {
      if ((*iter)->next)
	out << "node" << (long) *iter << ":next:e -> node" << (long) (*iter)->next << ":w;" << endl;
      if ((*iter)->prev)
	out << "node" << (long) *iter << ":prev:w -> node" << (long) (*iter)->prev << ":e [weight=0];" << endl;
    }

    // Draw iterators
    for (typename imap::iterator it = iterators.begin(); it != iterators.end(); ++it) {
      out << it->second << " -> node" << (long) it->first->node << "[label=\"node\"];" << endl;
    }

    out << "}" << endl;

    string command = "dot " + name + ".dot -T ps -o " + name + ".ps";
    popen(command.c_str(), "r");

    if (pause) {
    cout << "Press Return to continue" << endl;
    string s;
    getline(cin,s);
    }
  }
};
