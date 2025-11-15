#ifndef AVLBST_H
#define AVLBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include "bst.h"

struct KeyError { };

/**
* A special kind of node for an AVL tree, which adds the balance as a data member, plus
* other additional helper functions. You do NOT need to implement any functionality or
* add additional data members or helper functions.
*/
template <typename Key, typename Value>
class AVLNode : public Node<Key, Value>
{
public:
    // Constructor/destructor.
    AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent);
    virtual ~AVLNode();

    // Getter/setter for the node's height.
    int8_t getBalance () const;
    void setBalance (int8_t balance);
    void updateBalance(int8_t diff);

    // Getters for parent, left, and right. These need to be redefined since they
    // return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
    // for more information.
    virtual AVLNode<Key, Value>* getParent() const override;
    virtual AVLNode<Key, Value>* getLeft() const override;
    virtual AVLNode<Key, Value>* getRight() const override;

protected:
    int8_t balance_;    // effectively a signed char
};

/*
  -------------------------------------------------
  Begin implementations for the AVLNode class.
  -------------------------------------------------
*/

/**
* An explicit constructor to initialize the elements by calling the base class constructor
*/
template<class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key& key, const Value& value, AVLNode<Key, Value> *parent) :
    Node<Key, Value>(key, value, parent), balance_(0)
{

}

/**
* A destructor which does nothing.
*/
template<class Key, class Value>
AVLNode<Key, Value>::~AVLNode()
{

}

/**
* A getter for the balance of a AVLNode.
*/
template<class Key, class Value>
int8_t AVLNode<Key, Value>::getBalance() const
{
    return balance_;
}

/**
* A setter for the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::setBalance(int8_t balance)
{
    balance_ = balance;
}

/**
* Adds diff to the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff)
{
    balance_ += diff;
}

/**
* An overridden function for getting the parent since a static_cast is necessary to make sure
* that our node is a AVLNode.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key, Value>*>(this->parent_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key, Value>*>(this->left_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getRight() const
{
    return static_cast<AVLNode<Key, Value>*>(this->right_);
}


/*
  -----------------------------------------------
  End implementations for the AVLNode class.
  -----------------------------------------------
*/


template <class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value>
{
public:
    virtual void insert (const std::pair<const Key, Value> &new_item); // TODO
    virtual void remove(const Key& key);  // TODO
protected:
    virtual void nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2);

    // Add helper functions here
    void rotateLeft(AVLNode<Key, Value>* n);
    void rotateRight(AVLNode<Key, Value>* n);
    void insertFix(AVLNode<Key, Value>* p, AVLNode<Key, Value>* n);
    void removeFix(AVLNode<Key, Value>* n, int diff);
    int height(AVLNode<Key, Value>* root);
};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert (const std::pair<const Key, Value> &new_item)
{
    // TODO
    //following the logic of our PPT
    if(this->root_==NULL){
      AVLNode<Key, Value>* n = new AVLNode<Key, Value>(new_item.first, new_item.second, NULL); //call constructor
      this->root_ = n;
      n->setBalance(0);
      return;
    } //if empty tree, we set n as the root and b(n)=0
    //similar BST search as in the insert function in BST
    //find what should be the new element's parent (i.e. where to insert and what to insert, since creating a new Node also requires a parent)
    Node<Key,Value>* newParent = this->root_;
    Node<Key,Value>* temp = this->root_; //still type Node because root_ is still Node
    while(temp!=NULL){
      newParent=temp; //we do it inside the loop because outside the loop temp would be NULL; we do it before iterating temp because otherwise in the last iteration we would give NULL to newParent
      if((temp->getKey())>(new_item.first)){
        temp=temp->getLeft();
      }
      else if((temp->getKey())<(new_item.first)){ //very standard search fashion
        temp=temp->getRight();
      }
      else{ //that those two keys are equal, we should overwrite
        temp->setValue(new_item.second);
        return;
      }
    }
    //insert the node with the determined parent
    AVLNode<Key,Value>* p = static_cast<AVLNode<Key, Value>*>(newParent); //with our just-figured-out parent
    AVLNode<Key,Value>* n = new AVLNode<Key, Value>(new_item.first,new_item.second,p);
    if((new_item.first)<(p->getKey())){
      p->setLeft(n);
    }
    else{ //only left with the case in which the key in the incoming node is greater than the parent
      p->setRight(n);
    }

    n->setBalance(0);
    //look at its parent
    if(p->getBalance()==-1){
      p->setBalance(0);
      return;
    }
    else if(p->getBalance()==1){ //by definition of a parent, we could only insert on the NULL branch of parent as a child, so in either 1 or -1 case of parent's balance, we are safe to clear it to a 0
      p->setBalance(0);
      return;
    }
    else if (p->getBalance()==0) {
      if(n==p->getLeft()){
        p->setBalance(-1);
      }
      else{
        p->setBalance(1);
      }
      insertFix(p,n); //fix
    }
}

/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::remove(const Key& key)
{
    // TODO
    //adapted from BST's remove and only tweaked a little bit for AVL
    AVLNode<Key,Value>* myNode = NULL;
    Node<Key,Value>* temp = this->root_; //again, we have to perform traversing in Node
    bool ifFound = false;
    while(temp!=NULL){ //very standard search fashion
      if((temp->getKey())>key){
        temp=temp->getLeft();
      }
      else if((temp->getKey())<key){
        temp=temp->getRight();
      }
      else{
        myNode = static_cast<AVLNode<Key, Value>*>(temp); //static_cast it back to AVLNode
        ifFound = true;
        break;
      }
    }
    if(ifFound==false){ //not found
      return;
    }
    //found
    if(myNode->getLeft()!=NULL && myNode->getRight()!=NULL){ //if it has 2 children
      Node<Key,Value>* pred_bst = BinarySearchTree<Key, Value>::predecessor(myNode);
      AVLNode<Key,Value>* pred = static_cast<AVLNode<Key, Value>*>(pred_bst); //static cast from Node to AVLNode
      nodeSwap(myNode,pred); //swap. Since myNode has both a left child and a right child, we're sure that it has a predecessor (that the predecessor is definitely not NULL)
    }
    //by definition, pred could not have a right child. So after swapping, we reduced this case to the case where myNode only has one child or no child

    //from now on, we can finally use AVLNode safely and correctly
    AVLNode<Key,Value>* p = myNode->getParent(); 
    AVLNode<Key,Value>* l = myNode->getLeft(); 
    AVLNode<Key,Value>* r = myNode->getRight(); 

    //AVL-specific diff calculation
    int diff=0;
    if(p!=NULL){
      if(myNode==p->getLeft()){ //the right subtree now has greater height
        diff = 1; 
      }
      else if(myNode==p->getRight()){ //the left subtree now has greater height
        diff = -1;
      }
    }

    //promote and delete for various cases. 
    if(l!=NULL && r==NULL){ //only a left child
      if (p==NULL){ //myNode was a root
        this->root_=l;
        l->setParent(NULL);
      }
      else{
        if(myNode==p->getLeft()){ //myNode was a left child
          p->setLeft(l);
        }
        else{ //myNode was a right child
          p->setRight(l);
        }
        l->setParent(p);
      }
      delete myNode;
    }
    else if(l==NULL && r!=NULL){ //only a right child
      if (p==NULL){ //myNode was a root
        this->root_=r;
        r->setParent(NULL);
      }
      else{
        if(myNode==p->getLeft()){ //myNode was a left child
          p->setLeft(r);
        }
        else{ //myNode was a right child
          p->setRight(r);
        }
        r->setParent(p);
      }
      delete myNode;
    }
    else if(l==NULL && r==NULL){ //no children
      if(p==NULL){
        this->root_=NULL;
      }
      else{
        if(myNode==p->getLeft()){
          p->setLeft(NULL);
        }
        else{
          p->setRight(NULL);
        }
      }
      delete myNode;
    }
    if(p!=NULL){
      removeFix(p,diff); //fix
    }
}

//helper function that determines the height of a specific node. Copied from the equal-paths assignment and slightly modified
template<class Key, class Value>
int AVLTree<Key, Value>::height(AVLNode<Key, Value>* root){
    if(root==NULL){
      return -1; //this is essentially impossible because we'll verify that in our outer function. For safety, we still keep this.
    }
    if(root->getLeft()==NULL && root->getRight()==NULL){ //we've arrived at the leaf
      return 0;
    }
    if(root->getLeft()!=NULL && root->getRight()==NULL){
      return 1+maxDepth(root->getLeft());
    }
    if(root->getLeft()==NULL && root->getRight()!=NULL){ //cases where we only have one path to go down
      return 1+maxDepth(root->getRight());
    }
    //if we have both left and right
    int leftD = maxDepth(root->getLeft());
    int rightD = maxDepth(root->getRight());
    if(leftD>=rightD){ //compare to get the maxDepth
      return 1+leftD;
    }
    else{
      return 1+rightD;
    }
}

//helper function rotateLeft that helps dealing with balance
template<class Key, class Value>
void AVLTree<Key, Value>::rotateLeft(AVLNode<Key, Value>* n){
    if(n==NULL){
      return;
    }
    AVLNode<Key, Value>* r = n->getRight();
    if(r==NULL){
      return; //in rotateLeft we only allow right child to replace the original parent. Now we have nothing to rotate
    }
    AVLNode<Key, Value>* leftOfRight = r->getLeft();
    AVLNode<Key, Value>* p = n->getParent();
    //all other nodes, such as the left child of n, and the right child of n's right child, still follows their parents as before. 
    n->setRight(leftOfRight);
    if(leftOfRight!=NULL){
      leftOfRight->setParent(n);
    }//detach and reattach the leaf (subtree) first, for safety
    //deals with r and n
    r->setLeft(n);
    n->setParent(r);
    //deals p and r
    if(p==NULL){
      this->root_ = r;
      r->setParent(NULL);
    }
    else{ //see if it should be placed on the left or the right
      if(p->getLeft()==n){
        p->setLeft(r);
        r->setParent(p);
      }
      else{
        p->setRight(r);
        r->setParent(p);
      }
    }
}

//helper function rotateRight that helps dealing with balance
template<class Key, class Value>
void AVLTree<Key, Value>::rotateRight(AVLNode<Key, Value>* n){
  //mirrored from rotateLeft
  if(n==NULL){
    return;
  }
  AVLNode<Key, Value>* l = n->getLeft();
  if(l==NULL){
    return;
  }
  AVLNode<Key, Value>* rightOfLeft = l->getRight();
  AVLNode<Key, Value>* p = n->getParent();

  n->setLeft(rightOfLeft);
  if(rightOfLeft!=NULL){
    rightOfLeft->setParent(n);
  }

  l->setRight(n);
  n->setParent(l);

  if(p==NULL){
    this->root_ = l;
    l->setParent(NULL);
  }
  else{
    if(p->getLeft()==n){
      p->setLeft(l);
      l->setParent(p);
    }
    else{
      p->setRight(l);
      l->setParent(p);
    }
  }
}

//helper insertFix
template<class Key, class Value>
void AVLTree<Key, Value>::insertFix(AVLNode<Key, Value>* p, AVLNode<Key, Value>* n){
  //following the logic of our PPT
  if(p==NULL || p->getParent()==NULL){
    return;
  }
  AVLNode<Key, Value>* g = p->getParent();
  if(p==g->getLeft()){ //first assume it's a left child
    g->updateBalance(-1);
    if(g->getBalance()==0){
      return;
    }
    else if(g->getBalance()==-1){
      insertFix(g,p); //recurse
    }
    else if(g->getBalance()==-2){
      if(n==p->getLeft()){ //Left-Left. Zig-Zig
        rotateRight(g);
        p->setBalance(0);
        g->setBalance(0);
      }
      else if(n==p->getRight()){ //Left-Right. Zig-Zag
        rotateLeft(p);
        rotateRight(g);
        if(n->getBalance()==-1){
          p->setBalance(0);
          g->setBalance(1);
          n->setBalance(0);
        }
        else if (n->getBalance()==0){
          p->setBalance(0);
          g->setBalance(0);
          n->setBalance(0);
        }
        else if (n->getBalance()==1){
          p->setBalance(-1);
          g->setBalance(0);
          n->setBalance(0);
        }
      }
    }
  }
  else if(p==g->getRight()){ //if it's a right child, we just flip left/right and +/-
    g->updateBalance(1);
    if(g->getBalance()==0){
      return;
    }
    else if(g->getBalance()==1){
      insertFix(g,p);
    }
    else if(g->getBalance()==2){
      if(n==p->getRight()){ //Right-Right. Zig-Zig
        rotateLeft(g);
        p->setBalance(0);
        g->setBalance(0);
      }
      else if(n==p->getLeft()){ //Right-Left. Zig-Zag
        rotateRight(p);
        rotateLeft(g);
        if(n->getBalance()==1){
          p->setBalance(0);
          g->setBalance(-1);
          n->setBalance(0);
        }
        else if (n->getBalance()==0){
          p->setBalance(0);
          g->setBalance(0);
          n->setBalance(0);
        }
        else if (n->getBalance()==-1){
          p->setBalance(1);
          g->setBalance(0);
          n->setBalance(0);
        }
      }
    }
  }
}

//helper function removeFix
template<class Key, class Value>
void AVLTree<Key, Value>::removeFix(AVLNode<Key, Value>* n, int diff){
    //following the logic of our PPT
    if(n==NULL){
      return;
    }
    AVLNode<Key, Value>* p = n->getParent();
    int ndiff=0; //next diff
    if(p!=NULL){
      if(n==p->getLeft()){
        ndiff=1;
      }
      else{
        ndiff=-1;
      }
    }
    if(diff==-1){
      if((n->getBalance()+diff)==-2){
          AVLNode<Key, Value>* c = n->getLeft();
          if(c->getBalance()==-1){ //zig-zig
            rotateRight(n);
            n->setBalance(0);
            c->setBalance(0);
            removeFix(p,ndiff);
          }
          else if(c->getBalance()==0){ //zig-zig
            rotateRight(n);
            n->setBalance(-1);
            c->setBalance(1);
            return;
          }
          else if(c->getBalance()==1){ //zig-zag
            AVLNode<Key, Value>* g = c->getRight();
            rotateLeft(c);
            rotateRight(n);
            if(g->getBalance()==1){
              n->setBalance(0);
              c->setBalance(-1);
              g->setBalance(0);
            }
            else if(g->getBalance()==0){
              n->setBalance(0);
              c->setBalance(0);
              g->setBalance(0);
            }
            else if(g->getBalance()==-1){
              n->setBalance(1);
              c->setBalance(0);
              g->setBalance(0);
            }
            removeFix(p,ndiff); //recurse
          }
      }
      else if((n->getBalance()+diff)==-1){
        n->setBalance(-1);
        return;
      }
      else if((n->getBalance()+diff)==0){
        n->setBalance(0);
        removeFix(p,ndiff); //recurse
      }
    }
    else if (diff==1){ //mirroring the diff==-1 case
      if((n->getBalance()+diff)==2){
        AVLNode<Key, Value>* c = n->getRight();
        if(c->getBalance()==1){ //zig-zig
          rotateLeft(n);
          n->setBalance(0);
          c->setBalance(0);
          removeFix(p,ndiff);
        }
        else if(c->getBalance()==0){ //zig-zig
          rotateLeft(n);
          n->setBalance(1);
          c->setBalance(-1);
          return;
        }
        else if(c->getBalance()==-1){ //zig-zag
          AVLNode<Key, Value>* g = c->getLeft();
          rotateRight(c);
          rotateLeft(n);
          if(g->getBalance()==-1){
            n->setBalance(0);
            c->setBalance(1);
            g->setBalance(0);
          }
          else if(g->getBalance()==0){
            n->setBalance(0);
            c->setBalance(0);
            g->setBalance(0);
          }
          else if(g->getBalance()==1){
            n->setBalance(-1);
            c->setBalance(0);
            g->setBalance(0);
          }
          removeFix(p,ndiff); //recurse
        }
      }
      else if((n->getBalance()+diff)==1){
        n->setBalance(1);
        return;
      }
      else if((n->getBalance()+diff)==0){
        n->setBalance(0);
        removeFix(p,ndiff); //recurse
      }
    }
}


template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}


#endif
