#ifndef NODE_LIST_H
#define NODE_LIST_H

#include <iostream>   // include to allow for debugging output
using namespace std;

/*
 * A full list implementation in the spirit of the std::list class.
 * A similar high-level idea is implemented in Chapter 6.2 of our text.
 */
template <typename Object>
class NodeList {

private:
    struct Node {                                 // a node in the list
	Object elem;                              // element
	Node*  prev;                              // prev pointer
	Node*  next;                              // next pointer
	Node(const Object& e = Object(), Node* p = NULL, Node* n = NULL)
	    : elem(e), prev(p), next(n) { }      // constructor
    };

    int n;              // number of user's items in list
    Node* header;       // head-of-list sentinel
    Node* trailer;      // tail-of-list sentinel

public:
    // Support for generating graphviz images for a list
    class visualizer;
    friend class visualizer;       // Give visualizer access to list internals

    /* Default constructor creates an empty list. */
    NodeList()
	: n(0), header(new Node()), trailer(new Node()) {
	header->next = trailer;
	trailer->prev = header;
    }

    /* Return the number of objects in the list. */
    int size() const {
	return n;
    }

    /* Determine if the list is currently empty. */
    bool empty() const {
	return n == 0;
    }

    /* Returns a const reference to the front object in the list. */
    const Object& front() const {
	return header->next->elem;
    }

    /* Return a const reference to the last object in the list. */
    const Object& back() const {
	return trailer->prev->elem;
    }

    /* Insert an object at the front of the list. */
    void push_front(const Object& e) {
	Node* t = new Node(e, header, header->next);
	header->next = t;        // header has new node after it
	t->next->prev = t;       // old front has new node before it
	n++;
    }

    /* Insert an object at the back of the list. */
    void push_back(const Object& e) {
	Node* t = new Node(e, trailer->prev, trailer);
	trailer->prev = t;       // trailer has new node before it
	t->prev->next = t;       // old back has new node after it
	n++;
    }

    /* Remove the front object from the list. */
    void pop_front()  {
	Node* old = header->next;   // node to remove
	header->next = old->next;   // bypass old in forward direction
	old->next->prev = header;   // bypass old in reverse direction
	n--;
	delete old;
    }

    /* Remove the back object from the list. */
    void pop_back()  {
	Node* old = trailer->prev;   // node to remove
	trailer->prev = old->prev;   // bypass old in reverse direction
	old->prev->next = trailer;   // bypass old in forward direction
	n--;
	delete old;
    }

    // --------------  Nested iterator class -------------------
    class iterator {
	friend class NodeList<Object>;   // give list class access
	friend class visualizer;

    private:
	typename NodeList<Object>::Node* node;     // the inner Node class
	iterator(Node* n) : node(n) { }            // create from node

    public:
	/* Default constructor gives invalid iterator */
	iterator() : node(NULL) { }

	/* Copy constructor */
	iterator(const iterator& other) : node(other.node) {}

	/* Return live reference to element */
	Object& operator*() const {
	    return node->elem;        // return the element (as live reference)
	}

	/* Return live pointer to element */
	Object* operator->() const {
	    return &(node->elem);              // return address of the element
	}

	/* This is the "prefix" increment operator */
	iterator& operator++() {
	    node = node->next;                 // first, advance the iterator
	    return *this;                      // return updated self
	}

	/* This is the "postfix" increment operator */
	iterator operator++(int) {
	    iterator initial = *this;          // Make copy of incoming iterator
	    ++(*this);                         // Advance (using prefix form)
	    return initial;                    // Return copy of original
	}

	/* This is the "prefix" decrement operator */
	iterator& operator--() {
	    node = node->prev;                 // first, retreat the iterator
	    return *this;                      // return updated self
	}

	/* This is the "postfix" decrement operator */
	iterator operator--(int) {
	    iterator initial = *this;          // Make copy of incoming iterator
	    --(*this);                         // Decrement (using prefix form)
	    return initial;                    // Return copy of original
	}

	bool operator==(const iterator& other) {
	    return node == other.node;
	}

	bool operator!=(const iterator& other) {
	    return node != other.node;
	}
    };  // end iterator class

    // --------------  Nested const_iterator class -------------------
    // Note: This is very similar to iterator class, but the element
    // access methods, operator* and operator->, return constant references
    // (not live references).
    class const_iterator {
	friend class NodeList<Object>;   // give list class access
	friend class visualizer;

    private:
	typename NodeList<Object>::Node* node;     // the inner Node class
	const_iterator(Node* n) : node(n) { }      // create from node

    public:
	/* Default constructor gives invalid iterator */
	const_iterator() : node(NULL) { }

	/* Copy constructor */
	const_iterator(const const_iterator& other) : node(other.node) {}

	/* Return const reference to element */
	const Object& operator*() const {
	    return node->elem;        // return the element (as const reference)
	}

	/* Return const pointer to element */
	const Object* operator->() const {
	    return &(node->elem);            // return const pointer to element
	}

	/* This is the "prefix" increment operator */
	const_iterator& operator++() {
	    node = node->next;                 // first, advance the iterator
	    return *this;                      // return updated self
	}

	/* This is the "postfix" increment operator */
	const_iterator operator++(int) {
	    const_iterator initial = *this;    // Make copy of incoming iterator
	    ++(*this);                         // Advance (using prefix form)
	    return initial;                    // Return copy of original
	}

	/* This is the "prefix" decrement operator */
	const_iterator& operator--() {
	    node = node->prev;                 // first, retreat the iterator
	    return *this;                      // return updated self
	}
	
	/* This is the "postfix" decrement operator */
	const_iterator& operator--(int) {
	    const_iterator initial = *this;    // Make copy of incoming iterator
	    --(*this);                         // Decrement (using prefix form)
	    return initial;                    // Return copy of original
	}

	bool operator==(const const_iterator& other) {
	    return node == other.node;
	}

	bool operator!=(const const_iterator& other) {
	    return node != other.node;
	}
    };  // end const_iterator class

    friend class iterator;       // Give iterator access to list internals
    friend class const_iterator; // Give const_iterator access to list internals

    iterator begin() {
	return iterator(header->next);
    } 

    const_iterator begin() const {
	return const_iterator(header->next);
    }

    iterator end() {
	return iterator(trailer);        // trailer serves as end position
    }

    const_iterator end() const {
	return const_iterator(trailer);  // sentinel serves as end position
    }

    /*
     * Insert an object immediately before the iterator position.
     * Note that it must be an iterator (as opposed to a const_iterator).
     * It returns an iterator to the newly inserted item.
     */
    iterator insert(iterator pos, const Object& e) {
	Node* successor = pos.node;           // current node goes after new one
	Node* predecessor = successor->prev;  // this node goes before new one
	Node* newest = new Node(e, predecessor, successor);
	predecessor->next = newest;
	successor->prev = newest;
	n++;
	return iterator(newest);
    }

    /*
     * erase the item at the given iterator.  Returns an iterator to
     * the position that follows the deleted item.
     */
    iterator erase(iterator p) {
	Node* old = p.node;                // node to remove
	Node* successor = old->next;
	Node* predecessor = old->prev;
	predecessor->next = successor;
	successor->prev = predecessor;
	delete old;
	n--;
	return iterator(successor);
    }

protected:
    void removeAll() {        // remove list contents (but leave sentinels)
	while (!empty()) pop_front();
    }

    void copyFrom(const NodeList& other) {     // copy from other
	// assumes that this is proper empty list
	for (const_iterator ci = other.begin(); ci != other.end(); ++ci)
	    push_back(*ci);
    }

public:
    /* Copy constructor */
    NodeList(const NodeList& other)
	: n(0), header(new Node()), trailer(new Node()) {
	header->next = trailer;
	trailer->prev = header;
	copyFrom(other);
    }

    /* Destructor */
    ~NodeList() {
	removeAll();              // get rid of contents between sentinels
	delete header;            // get rid of sentinels as well
	delete trailer;
    }

    /* Assignment operator */
    NodeList<Object>& operator=(const NodeList& other) {
	if (this != &other) {                       // avoid self copy (x = x)
	    removeAll();                              // remove old contents
	    copyFrom(other);                          // copy new contents
	}
	return *this;
    }


    /*
     * Merges other into the list by moving all of its elements into
     * the container at their respective ordered positions. This
     * effectively removes all the elements in other (which becomes
     * empty), and inserts them into the container (which expands in
     * size by the number of elements moved).
     *
     * The entire operation does not involve the construction,
     * destruction or copy of any element object. They are moved.
     *
     * The pointers, references and iterators that referred to moved
     * elements keep referring to those same elements, but iterators
     * now iterate into the container the elements have been
     * transferred to.
     */
    void merge(NodeList& other) {
        Node* first = header->next;             // Pointer to first elem of list 1
        Node* second = other.header->next;      // Pointer to first elem of list 2
        while (second != other.trailer) {
            while (first != trailer) {
                if(second->elem <= first->elem) {   // if ptr2 should merge in
                    first->prev->next = second;
                    second->prev = first->prev;
                    first->prev = second;
                    other.header->next = second->next;
                    second = second->next;
                    second->prev->next = first;
                    ++n;                            // Don't forget to increment n!
                    --other.n;
                }
                if(second->elem > first->elem)      // if ptr2 shouldn't merge in
                    first = first->next;
            }
            first = trailer->prev;              // put the rest of list 2 in 1
            first->next = second;
            second->prev = first;
            trailer->prev = second;
            other.header->next = second->next;
            second->next = trailer;
            trailer->prev = second;
            second = other.header->next;
            ++n;                                    // Don't forget to increment n!
            --other.n;
        }
        other.trailer->prev = other.header;         // Force list 2 back together.
    }

};  // end of NodeList class

#include "NodeListVisualizer.tcc"   // code for visualizer
#endif
