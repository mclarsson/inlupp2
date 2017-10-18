#include <stdio.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "list.h"
#include "tree.h"
#include "db.c"
//#include "CUnit/Basic.h"



char *name[] = {"Krita", "Kanin", "Dator", "Äpple", "Fanta", "Bok", "Garderob", "Lampa", "Sax", "Pizza"};
char *desc[] = {"Trubbig", "Lätt", "Len", "Askalas", "Snuskig", "Ostig", "Söt", "Vit", "Blöt", "Fluffig"};
int price[] = {3, 5, 7, 11, 13, 1337, 1234567890, 98765431, 69, 42};
char *shelf[] = {"A1", "B2", "C3", "F12", "K37", "H54", "M8", "L82", "S32", "Y66"};
int amount[] = {1, 22, 333, 4444, 5555, 999999999, 1234, 56789, 0, 15};


int init_suite(void)
{
  return 0;
}


int clean_suite(void)
{
  return 0;
}

//--------------------------------------------------------------------------------------------
// List

void test_list_new(void)
{
  list_t *list = list_new(NULL);
  CU_ASSERT_TRUE( list != NULL );
}

/*
void test_link_new(void)
{
  link_t *linktest = link_new((list_value_t) 1, NULL);
  CU_ASSERT_TRUE( linktest->value.i == 1);  
}
*/

list_t *test_list_append(void)
{
   list_t *list = list_new(NULL);
  for (int i = 0; i < 10 ; i++)
    {
      list_value_t tmp = { .p = name[i] };
      list_append(list, tmp);
    }
  return list;
}

list_t *test_list_prepend(void)
{
 
   list_t *list = list_new(NULL);
  for (int i = 0; i < 10 ; i++)
    {
      list_value_t tmp = { .p = name[i] };
      list_prepend(list, tmp);
    }
  return list; 
}


list_t *test_list_insert(void)
{
  list_t *list = list_new(NULL);
  for (int i = 0; i < 10 ; i++)
    {
      list_value_t tmp = { .p = name[i] };
      list_insert(list, i, tmp);
    }
  return list;
}

void test_list_length(void)
{
  list_t *list = test_list_append();
  CU_ASSERT_TRUE(list_length(list) == 10);
  CU_ASSERT_TRUE(list_length(NULL) == 0);
}


void test_list_get(void)
{
  list_t *listappend = test_list_append();
  for (int i = 0 ; i < list_length(listappend); i++)
  {
    list_value_t tmp1 = { .p = name[i] };
    CU_ASSERT_TRUE( list_get(listappend, i).p == tmp1.p );
  }

  list_t *listprepend = test_list_prepend();
  for (int i = 0 ; i < list_length(listprepend); i++)
  {
    list_value_t tmp2 = { .p = name[i] };
    CU_ASSERT_TRUE( list_get(listprepend, i).p == tmp2.p );
  }

  list_t * listinsert = test_list_insert();
  for (int i = 0 ; i < list_length(listinsert); i++)
  {
    list_value_t tmp3 = { .p = name[i] };
    CU_ASSERT_TRUE( list_get(listinsert, i).p == tmp3.p );
  }
}

/*
void test_list_remove(void)
{
  list_t *list = test_list_append();
  list_value_t tmp = { .p = "Krita" };
  CU_ASSERT_TRUE( list_remove(list, 0, tmp));
}
*/

/*
void test_list_delete(void)
{
  list_t *list = test_list_append();
  list_value_t elem;
  list_delete(list, (list_action) cleanup(elem));
}
*/                







//--------------------------------------------------------------------------------------------
// Tree

tree_t *test_tree_new(void)
{
  tree_t *tree = tree_new(NULL);
  for (int i = 0; i < 10; i++)
    {
      tree_key_t tmpkey = { .p = name[i] };
      tree_value_t tmpvalue = { .p = desc[i] };
      tree_insert(tree, tmpkey, tmpvalue);
    }
  return tree;
}

void test_tree_size(void)
{
  tree_t *tree = test_tree_new();
  CU_ASSERT_TRUE(tree_size(tree) == 10);
  CU_ASSERT_TRUE(tree_new(NULL) == 0);
}


void test_tree_depth(void)
{
  tree_t *tree = test_tree_new();
  CU_ASSERT_TRUE(tree_depth(tree) == 3);
  CU_ASSERT_TRUE(tree_depth(tree_new(NULL)) == 0);
}

void test_tree_has_key(void)
{
  tree_t *tree = test_tree_new();
  for (int i = 0; i < tree_size(tree); i++)
    {
      tree_key_t tmpkey = { .p = name[i] };
      CU_ASSERT_TRUE(tree_has_key(tree, tmpkey));
    }
}


void test_tree_get(void)
{
  tree_t *tree = test_tree_new();
  for (int i = 0; i < tree_size(tree); i++)
    {
      tree_key_t tmpkey = { .p = name[i] };
      tree_value_t tmpvalue = { .p = desc[i] };
      CU_ASSERT_TRUE(tree_get(tree, tmpkey) == tmpvalue);
    }
  
}


void test_tree_insert(void)
{
  CU_ASSERT_TRUE(tree_size(test_tree_new())== 10);
}



//--------------------------------------------------------------------------------------------
// Database


void test_add_goods(tree_t *tree)
{
  for (int i = 0; i < 10; i++)
    {
      list_value_t tmp_name = { .p = name[i] };
      list_value_t tmp_desc = { .p = desc[i] };
      list_value_t tmp_price = { .i = price[i] };
      list_value_t tmp_shelf_name = { .p = shelf[i] };
      list_value_t tmp_shelf_amount = { .i = amount[i] };
      list_value_t item = (list_value_t) make_item((char *) tmp_desc, (int) tmp_price).p;
      list_value_t shelf = { .p = make_shelf((char *)tmp_shelf_name, (int) tmp_shelf_amount) };
      goods_t *tmp_goods = make_goods(tmp_name.p, item.p, shelf);
      tree_insert(tree, (tree_key_t) tmp_name.p, (tree_value_t) item.p);
      CU_ASSERT_TRUE(tree_has_key(tree, (tree_key_t) tmp_name.p));
     }
}



void test_remove_goods(void)
{
  remove_goods(
}


void test_undo_goods(void)
{
  test_add_goods();
  test_remove_goods();
  CU_ASSERT_TRUE( undo_action(test_remove_goods()) == test_add_goods());
}


