/*
 * Стуктура Декартово дерево по неявному ключу.
 * Операции
 *          move - перестановка отрезка(в условном массиве)
 *          add - прибавить число на отрезке
 *          get_sum - узнать сумму на отрезке
 *          print - вывод декартового дерева в порядке InOrder
 *          update - обновить узел
 *          push - Проталкивание обещания
 *          add_promise - добавление обещания
 *          insert - добавить
 *          erase - удалить по индексу
 *          depth - глубина дерева
 *
 *
 */

#include <iostream>

using namespace std;

class Treap {
public:
    Treap *left, *right;
    int val;
    int priority;
    int size;
    int sum;
    int promise;

    Treap(int val = 0) {
        this->val = val;
        left = nullptr;
        right = nullptr;
        size = 1;
        sum = val;
        promise = 0;
        priority = ((rand() << 16) ^ rand()); // Циклический сдвиг на 16.
    }
    ~Treap() {
        if (left)
            delete left;
        if (right)
            delete right;
    }


    static int get_size(Treap *v) {
        if (v == nullptr)
            return 0;
        return v->size;
    }

    static int get_sum(Treap *v) {
        if (v == nullptr)
            return 0;
        return v->sum;
    }

    static void add_promise(Treap *v, int p) {   //Добавление обещания
        if (v == nullptr)
            return;
        v->promise += p;
    }

    static void push(Treap *v) { // Проталктвание обещания
        if (v == nullptr)
            return;
        v->sum += v->size * v->promise;         // Сумма в вершине с обещнанием
        v->val += v->promise;                   // Учитываем обещание
        add_promise(v->left, v->promise);       // Добавление в левого ребенка
        add_promise(v->right, v->promise);      // Добавление в правого ребенка
        v->promise = 0;                         //Обещание выполнено
    }

    static void update(Treap *v) {
        if (v == nullptr)
            return;
        push(v->left);
        push(v->right);
        v->size = get_size(v->left) + get_size(v->right) + 1;
        v->sum = get_sum(v->left) + get_sum(v->right) + v->val;

    }

    static Treap* merge(Treap *root_l, Treap *root_r) {
        push(root_l); //for
        push(root_r); //push
        if (root_l == nullptr)
            return root_r;
        if (root_r == nullptr)
            return root_l;

        if (root_l->priority < root_r->priority) {
            root_r->left = merge(root_l,root_r->left);
            update(root_r);
            return root_r;
        } else {
            root_l->right = merge(root_l->right, root_r);
            update(root_l);
            return root_l;
        }
    }

    static pair<Treap *, Treap *> split(Treap *root, int sz) {
        if (root == nullptr)
            return {nullptr, nullptr};
        push(root); // for push
        if (get_size(root->left) >= sz) {
            auto p = split(root->left, sz);
            root->left = p.second;
            update(root);
            return {p.first, root};
        } else {
            auto p = split(root->right, sz - get_size(root->left) - 1);
            root->right = p.first;
            update(root);
            return {root, p.second};
        }
    }

    static void print(Treap *root) {
        if (root == nullptr)
            return;
        push(root);
        print(root->left);
        cout << root->val << " ";
        print(root->right);
    }

    static int get_by_index(Treap* root, int K) {
        while (root != nullptr)
        {
            int sizeLeft = get_size(root->left);

            if (sizeLeft == K)
                return root->val;

            root = sizeLeft > K ? root->left : root->right;
            if (sizeLeft < K)
                K -= sizeLeft + 1;
        }
        return -1;
    }

    static void move(Treap* &root, int l, int r) {   // Перенос отрезка в начало(условного массива). O(logN) indexing starts at 1
        auto p1 = split(root, r);
        auto p2 = split(p1.first, l);
        root = merge(p2.second, p2.first);
        root = merge(root, p1.second);
    }

    static int get_sum(Treap* &root, int l, int r) {     //Сумма на отрезке. O(logN) indexing starts at 1
        auto p1 = split(root, r);
        auto p2 = split(p1.first, l);
        int res = get_sum(p2.second);
        root = merge(p2.first, p2.second);
        root = merge(root, p1.second);
        return res;
    }

    static void add(Treap* &root, int l, int r, int q) {  // Добавить на отрезке. Log(N) indexing starts at 1
        auto p1 = split(root, r);
        auto p2 = split(p1.first, l);
        add_promise(p2.second, q);
        root = merge(merge(p2.first, p2.second), p1.second);
    }

    static Treap* add_node(Treap* root, int val){ // Добавление, возвращаем новый корень.
        auto p = split(root, val);
        Treap* temp = new Treap(val);
        temp = merge(merge(p.first, temp), p.second);
        update(temp);
        return temp;
    }

    static Treap* insert(Treap* root, int index, int val){ // Добавление по индексу(Вставка за логарифм), возвращаем новый корень.
        auto p1 = split(root, index - 1);
        Treap* temp = new Treap(val);
        return merge(merge(p1.first, temp), p1.second);
    }

    static Treap* erase(Treap* root, int pos) {  // Удаление по индексу. indexing starts at 1
            auto p1 = split(root, pos - 1);
            auto p2 = split(p1.second, 1);
            return merge(p1.first, p2.second);
        }

    static int depth(Treap* root) {  // Оценка глубины дерева.
        if (root == nullptr)
            return 0;
        return 1 + max(depth(root -> left), depth(root -> right));
    }


};

int main() {
    Treap *root = nullptr; //Глобальный корень
    cout << "Enter counts of elements and fill them:";
    int n;
    cin >> n;
    for (int i = 0; i < n; i++) {
        int a;
        cin >> a;
        root = Treap::add_node(root, a);
    }

    Treap::print(root);
    cout << endl;
    cout << "Indexing starts at 1 \n"
         << "? - sum on [l, r]:  ? l r " << "\n"
         <<"m - move [l , r] to begin:  m l r "<< "\n"
         <<"+ add on [l, r]:   + l r" << "\n"
         <<"a - add: a value;g - get by index: g index;\ni - insert by index(begin at 1): i index value;\ne - erase by index(begin at 1): e index"<< "\n"
         <<"p - print; d - depth; q - exit" << endl;

    while (true) {
        char c; cin >> c;
        if (c == '?') {
            int l, r;
            cin >> l >> r;
            cout << Treap::get_sum(root, l - 1, r);
            cout << endl;
        }
        if (c == 'g') {
            int k; cin >> k;
            cout << Treap::get_by_index(root, k);
            cout << endl;
        }
        if (c == 'm') {
            int l, r;
            cin >> l >> r;
            Treap::move(root, l - 1, r);
        }
        if (c == '+') {
            int l ,r, q;
            cin >> l >> r >> q;
            Treap::add(root, l - 1, r, q);
        }
        if (c == 'p') {
            Treap::print(root);
            cout << endl;
        }
        if (c == 'd') {
            cout << Treap::depth(root);
            cout << endl;
        }
        if (c == 'a') {
            int val; cin  >> val;
            root = Treap::add_node(root, val);
        }
        if (c == 'i') {
            int index, val; cin  >> index >> val;
            root = Treap::insert(root, index, val);
        }
        if (c == 'e') {
            int e; cin >> e;
            root = Treap::erase(root, e);
        }
        if (c == 'q')
            break;
    }
    return 0;
}
