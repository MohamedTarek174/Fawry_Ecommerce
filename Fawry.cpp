/*
Name : Mohamed Tarek Abdelsattar

This code in c++
there are main 6 classes:
1. Product
2. IShippable: An interface for shippable items.
3. CartItem
4. Customer
5. ShippingServic: Handles the shipping of items in the cart
6. ShoppingCart
*/
#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <ctime>

using namespace std;



class Product {
protected:
    string name;
    double price;
    int stock;
    bool expirable;
    bool shippable;
    time_t expiryDate;
    double weight;

public:
    Product(string name, 
            double price, 
            int stock,
            bool expirable = false, 
            bool shippable = false,
            time_t expiryDate = 0, 
            double weight = 0.0)
        {
        this->name = name;
        this->price = price;
        this->stock = stock;
        this->expirable = expirable;
        this->shippable = shippable;
        this->expiryDate = expiryDate;
        this->weight = weight; // all in kg
        }

    string getName() const { return name; }
    double getPrice() const { return price; }
    int getStock() const { return stock; }

    bool isExpirable() const { return expirable; }
    bool isExpired() const {
        if (!expirable) return false;
        time_t now = time(0);
        return difftime(expiryDate, now) < 0;
    }

    bool isShippable() const { return shippable; }
    double getWeight() const { return weight; }

    void reduceStock(int quantity) {
        stock -= quantity;
        if (stock < 0) stock = 0;
    }
};

class IShippable {
public:
    virtual string getName() const = 0;
    virtual double getWeight() const = 0;
    virtual ~IShippable() {}
};

class CartItem {
private:
    Product product;
    int quantity;

public:
    CartItem(const Product& product, int quantity)
    : product(product), quantity(quantity) {}


    const Product& getProduct() const { return product; }
    int getQuantity() const { return quantity; }

    double getTotalPrice() const {
        return product.getPrice() * quantity;
    }

    bool isShippable() const {
        return product.isShippable();
    }

    double getTotalWeight() const {
        return product.getWeight() * quantity;
    }

    bool isExpired() const {
        return product.isExpired();
    }
};

class Customer {
private:
    string name;
    double balance;

public:
    Customer(string name, double balance)
    {
        this->name = name;
        this->balance = balance;
    }

    string getName() const { return name; }
    double getBalance() const { return balance; }

    bool availableBalance(double amount) const {
        return balance >= amount;
    }

    void pay(double amount) {
        balance -= amount;
    }
};

class ShippingService {
public:
    void shipItems(const vector<CartItem>& items) {
        cout<<"** Shipment notice **"<<endl;
        double totalWeight = 0;

        for (const CartItem& item : items) { // Only shippable items
            if (item.isShippable()) {
                cout<<item.getQuantity()<<"x "<<item.getProduct().getName() <<" "<<static_cast<int>(item.getProduct().getWeight() * 1000)<<"g"<<endl;

                totalWeight += item.getTotalWeight();
            }
        }


        cout<<"Total package weight "<<totalWeight<<"kg"<<endl;
    }
};

class ShoppingCart {
private:
    vector<CartItem> items;
    double shippingFee; 

public:
    void addItem(const Product& product, int quantity) {
        if (quantity <= 0 || quantity > product.getStock()) {
            cout<<"Invalid quantity for "<<product.getName()<<endl;return;
        }
        items.emplace_back(product, quantity);
    }
    void addShippingFee(double fee) {
        shippingFee = fee;
    }

    void checkout(Customer& customer) {
        if (items.empty()) {
            cout<<"Your cart is empty , Start Shopping"<<endl;return;
        }

        double subtotal = 0;
        bool needsShipping = false;
        vector<CartItem> shippableItems;

        for (const CartItem& item : items) {
            if (item.isExpired()) {
                cout<<"Error: "<<item.getProduct().getName()<<" is expired"<<endl;return;
            }
            if (item.getQuantity() > item.getProduct().getStock()) {
                cout<<"There is only "<<item.getProduct().getStock() << " for " <<item.getProduct().getName()<<endl;return;
            }
            subtotal += item.getTotalPrice();

            if (item.isShippable()) {
                needsShipping = true;
                shippableItems.push_back(item);
            }
        }

        double total = subtotal + (needsShipping ? shippingFee : 0);
        if (!customer.availableBalance(total)) {
            cout<<"No Availabe balance"<<endl;return;
        }

        if (needsShipping) {
            ShippingService ss;
            ss.shipItems(shippableItems);
        }

        customer.pay(total);

        cout<<"** Checkout receipt **"<<endl;
        for (const CartItem& item : items) {
            cout<<item.getQuantity()<<"x "<<item.getProduct().getName()<<" "<<static_cast<int>(item.getTotalPrice())<<endl;
        }

        cout<<"----------------------"<<endl;
        cout<<"Subtotal "<<static_cast<int>(subtotal)<<endl;
        cout<<"Shipping "<<static_cast<int>(needsShipping ? shippingFee : 0)<<endl;
        cout<<"Amount "<<static_cast<int>(total)<<endl;
        cout<<"Balance "<<static_cast<int>(customer.getBalance())<<endl;
    }
};

int main() {
    time_t now = time(0);
    time_t future = now + 5 * 24 * 60 * 60;

    Product cheese("Cheese", 100, 10, true, true, future, 0.2);
    Product biscuits("Biscuits", 150, 5, true, true, future, 0.7);
    Product scratchCard("Mobile Card", 50, 20);
    Product tv("TV", 3000, 2, false, true, 0, 10.0);

    Customer customer("Mohamed", 4500);
    
    cout<<endl;
    
    ShoppingCart cart;
    cart.addShippingFee(50); // demo shipping fee

    cart.addItem(cheese, 2);
    cart.addItem(biscuits, 3);
    cart.addItem(tv, 1);
    cart.addItem(scratchCard, 3);

    cart.checkout(customer);
    return 0;
}
