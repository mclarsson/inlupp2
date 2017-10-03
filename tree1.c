#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <ctype.h>
#include <string.h>
#include "tree.h"
#include "utils.h"
#include "list.h"



struct tree // Struct for a tree, which recursively creates more nodes.
{
  K key;
  T elem;
  tree_t *right;
  tree_t *left;
} tree;

struct item
{
  char *name;
  char *desc;
  int price;
  list_t *shelves;
};

typedef struct item item_t;


tree_t *tree_new() // Allocates memory on the heap for our tree.
{
  tree_t *alloc = (tree_t*) calloc(1, sizeof(tree_t));
  return alloc;
}

void tree_delete(tree_t *tree) // "Deletes" the tree by traversing the tree from the bottom to the top, freeing the memory from the heap.
{
  if (tree != 0)
    {
      tree_delete(tree->left);
      tree_delete(tree->right);
      free(tree);
    }
}


int tree_size(tree_t *tree)
{
  if (tree == NULL)
    {
      return 0;
    }
  else
    {
      return (tree_size(tree->left) + tree_size(tree->right) + 1);
    }
}


int tree_depth(tree_t *tree) // Checks to see the depth of the tree, traversing the left subtree and then the right subtree.
{
  if (tree == NULL)
    {
      return 0;
    }
  else
    {
      int left_d = tree_depth(tree->left);
      int right_d = tree_depth(tree->right);  
      if (left_d < right_d)
        {
          return right_d + 1;
        }
      else
        {
          return left_d + 1;
        }
     
    }
}
      

bool tree_insert(tree_t  **tree, K key, T elem)   // Inserts an item into the tree, returning a bool for if it was a success.
{
 if (*tree == NULL) //->left == NULL && tree->right == NULL)
    {
      // If the tree is nonexistent it will allocate memory for it and then create an empty tree.
      *tree = tree_new();
      (*tree) -> key = key; 
      (*tree) -> elem  = elem;
      (*tree) -> left = NULL;
      (*tree) -> right = NULL;
    }
      
 else if (strcmp((*tree)->key, key) > 0)  //If the new value is lesser than the root the new value will be put in the left subtree.
    {
      tree_insert(&(*tree)->left, key, elem);
     return true;
    }
 else if (strcmp((*tree)->key, key) < 0)  //If the new value is greater than the root the new value will be put in the right subtree.
    {
      tree_insert(&(*tree)->right, key, elem);
      return true;
    }
  else
    {
      return false;
    }

  return true;
}


tree_t *tree_maker(tree_t  *tree, K key, T elem)   // Inserts an item into the tree, returning a bool for if it was a success.
{
  if (tree == NULL)
    {
      tree_t *tmp;
      tmp = (tree_t*) calloc(1, sizeof(tree_t));
      tmp -> key = key; // If the tree is nonexistent it will allocate memory for it and then create an empty tree.
      tmp -> elem = elem;
      tmp -> left = NULL;
      tmp -> right = NULL;

      return tmp;
    }
      
  else if (strcmp(tree->key, key) > 0)  //If the new value is lesser than the root the new value will be put in the left subtree.
    {
      tree->left = tree_maker(tree->left, key, elem); 

    }
  else if (strcmp(tree->key, key) < 0)  //If the new value is greater than the root the new value will be put in the right subtree.
    {
      tree->right = tree_maker(tree->right, key, elem);

    }
  else
    {
      return tree;
    }

  return tree;
}

void appendInorder(tree_t *tree, list_t *newlist)
{
      if (tree == NULL)
        {
          return;
        }
      {
        appendInorder(tree->left, newlist);
        list_append(newlist,tree->elem);
        appendInorder(tree->right, newlist);
      }   
}

void appendKeyInorder(tree_t *tree, list_t *newlist)
{
      if (tree == NULL)
        {
          return;
        }
      {
        appendKeyInorder(tree->left, newlist);
        list_append(newlist,tree->key);
        appendKeyInorder(tree->right, newlist);
      }   
}




void PrintInorder(tree_t *tree, int *index)
{
  int maxnumb = 20;
  list_t *newlist = list_new();
  if (tree == NULL)
    {
      return;
    }
  if (tree_size(tree) <= maxnumb)
    {
      PrintInorder(tree->left, index);
      ++*index;
      printf("%d: %s\n", *index ,tree->key);
      PrintInorder(tree->right, index);
    }
  else
    while (*index < maxnumb)
      {
        appendInorder(tree, newlist);
        ++*index;
        item_t *item = ((item_t*) list_get(newlist, *index - 1));
        printf("%d: %s\n", *index, item->name);
        free(newlist);
      }
}
   



bool tree_has_key(tree_t *tree, K key) // Checks if a specific item exists in the tree.
{
  if (tree == NULL)
    {
      return false;
    }
  else if (strcmp(tree->key, key) == 0)
    {
      return true;
    }
  else if (strcmp(tree->key, key) > 0)
    {
      return tree_has_key(tree->left, key);
    }
  else
    {
      return tree_has_key(tree->right, key);
    }
}


T tree_get(tree_t *tree, K key)
{
  if (tree == NULL)
    {
      return NULL;
    }

  else if ((strcmp(tree->key, key)) == 0)
    {
      return tree->elem;
    }
      
  else if (strcmp(tree->key, key) > 0)
    {
      return tree_get(tree->left, key);
    }
  
  else //if  (strcmp(tree->K, key) < 0)
    {
      return tree_get(tree->right, key);
    }
}

/*
K *node_keys(tree_t tree, K array, int *index)
{
  
}

K *tree_keys(tree_t *tree)
{
  int i = 0;
  tree_t *cursor = tree;
  tree_t *cursor2 = tree;
  K array[] = {};
  if(cursor == NULL)
    {
      return array;
    }
  for (i; i <= tree_size(tree); ++i)
    {
      if (cursor->left != NULL)
        {
          array[i] = cursor->key;
        }
      else if (cursor->right != NULL)
        {
          
        }
    }

  
  
     array[i] = tree->key;
     ++*i;
     if(tree->left != NULL)
       {
          tree_keys(tree->left);
       }
     if(tree->right != NULL)
       {
          tree_keys(tree->right);
       }
       return array;
}




int main()
{  
  tree_t *tester = tree_maker(NULL, "F", "Hubbabubba");
  tree_maker(tester, "B", 2);
  tree_maker(tester, "C", 3);
  tree_maker(tester, "E", 5);
  tree_maker(tester, "D", 4);
  tree_maker(tester, "A", 1);
   
  PrintInorder(tester);

  int asd = tree_depth(tester);
  printf("So deep, much wow! %d\n", asd);
  int loj = tree_get(tester, "F");
  printf("Fanns den?: %d\n", loj);
    
  bool hej = tree_has_key(tester, "B");
  printf("Hittar vi den?: %d\n", hej);
  tree_delete(tester);
  return 0;

}


*/
