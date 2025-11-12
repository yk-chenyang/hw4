#ifndef RECCHECK
//if you want to add any #includes like <iostream> you must do them here (before the next endif)

#endif

#include "equal-paths.h"
using namespace std;


// You may add any prototypes of helper functions here
int maxDepth(Node * root); //helper function that determines the max depth (height) of a tree with the current node as the root

bool equalPaths(Node * root)
{
    // Add your code below
    if(root==NULL){
      return true; //true if we have absolutely nothing
    }
    if(root->left==NULL && root->right==NULL){
      return true; //true if the current node is a leaf node
    }
    if(root->left!=NULL && root->right==NULL){
      return equalPaths(root->left);
    }
    if(root->left==NULL && root->right!=NULL){ //cases where we only have one choice to go down
      return equalPaths(root->right);
    }
    if(maxDepth(root->left)!=maxDepth(root->right)){ //now with the assumption that already we've already satisfied: root->left!=NULL && root->right!=NULL
      return false;
    }
    return equalPaths(root->left) && equalPaths(root->right); //if we have two nodes to go to and it seems their maxDepths are the same, we go to each to check the specific
}

int maxDepth(Node * root){
    if(root==NULL){
      return -1; //this is essentially impossible because we'll verify that in our outer function. For safety, we still keep this.
    }
    if(root->left==NULL && root->right==NULL){ //we've arrived at the leaf
      return 0;
    }
    if(root->left!=NULL && root->right==NULL){
      return 1+maxDepth(root->left);
    }
    if(root->left==NULL && root->right!=NULL){ //cases where we only have one path to go down
      return 1+maxDepth(root->right);
    }
    //if we have both left and right
    int leftD = maxDepth(root->left);
    int rightD = maxDepth(root->right);
    if(leftD>=rightD){ //compare to get the maxDepth
      return 1+leftD;
    }
    else{
      return 1+rightD;
    }
}