// �������� ��� � ������ � ����� � C++ ��� RTTI

#include <atomic>
#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include <stdexcept>

using namespace std;

// ����� ��� �������� ������������ ������ X �� ������ Y
template <typename X>
struct is_derived
{
    template <typename Y>
    static bool from() noexcept
    {
        return X::id() == Y::id() ||
               is_derived<typename X::base>::template from<Y>();
    }
};

// ����� ������������ ����� ���������� ������
// ������������ �������������� ������, ���
// ����������� ������ ���� �� ����������.
// ���������� ������ atomic � �������������
// ������ � ������������� ������ class_id.
namespace
{
    atomic<int> g_last_class_index;
}

// ����� ���������� ������������� ������ X ����� X::id().
// ������������ ��������� � ������ ���� �������� ��� ������� ������.
class class_id
{
public:
    class_id(const char* const class_name) noexcept
        : m_name(class_name), m_index(++g_last_class_index) { }

    const char* const name() const noexcept { return m_name; }
    int index() const noexcept { return m_index; }

    bool operator == (const class_id& another) const noexcept
    {
        return m_index == another.m_index;
    }

private:
    const char* const m_name;
    int m_index;
};

// ������� ����� �������� ����� ���� �������.
class creature
{
public:
    creature() noexcept { }
    virtual ~creature() noexcept { }

    static const class_id& id() noexcept;
    virtual const class_id& who() const noexcept { return id(); }

    template <typename T>
    bool is() const noexcept
    {
        return is_base_id(T::id());
    }

    virtual bool is_base_id(const class_id& base_id) const noexcept
    {
        return base_id == id();
    }

    template <typename T>
    void assert_is() const
    {
        if (!is<T>())
            throw runtime_error("Creature is not inherited from this class of creatures.");
    }

    template <typename T>
    const T& as() const
    {
        assert_is<T>();
        return static_cast<const T&>(*this);
    }

    template <typename T>
    T& as()
    {
        assert_is<T>();
        return static_cast<T&>(*this);
    }
};

// �������� �������� ������������ ����� X::base
// ������ ����������� �� ������� ������.
template <>
struct is_derived<creature>
{
    template <typename Y>
    static bool from() noexcept
    {
        return creature::id() == Y::id();
    }
};

// ��� ����������� ��������� ����, ������� ��������
// ��������� ����������: who() � is_base_id(), � �����
// ��������� base ����� � ����� �������������� ������.
#define DERIVED_FROM(base_class) \
    typedef base_class base; \
    virtual bool is_base_id(const class_id& base_id) const noexcept override \
    { \
        return base_id == id() || base::is_base_id(base_id); \
    } \
    virtual const class_id& who() const noexcept override \
    { \
        return id(); \
    } \
    static const class_id& id() noexcept

// ���� �������� ��������� ������� ��� �������� ���������
// ����������� ������ �������� � ������������� ������������.

// ����� ���� �������� �����, � ������� ���� ��� � �����.
class animal : public creature
{
public:
    animal(const string& animal_name)
        : m_name(animal_name) { }

    const string& name() const noexcept { return m_name; }

    virtual string say() const noexcept = 0;

    DERIVED_FROM(creature);

private:
    string m_name;
};

// ������ ����� ���� ������, � ����� � �� ����, �� ������� �����.
class cat : public animal
{
public:
    cat(const string& cat_name, const bool is_cute)
        : base(cat_name), m_cute(is_cute) { }

    bool cute() const noexcept { return m_cute; }

    virtual string say() const noexcept override
    {
        return (cute() ? "Meow!" : "MEEEAAAOOOOOW!!!");
    }

    DERIVED_FROM(animal);

private:
    bool m_cute;
};

// ������ ����� ���� ������� � ����� ������.
class dog : public animal
{
public:
    dog(const string& dog_name, unsigned dog_strength)
        : base(dog_name), m_strength(dog_strength) { }

    unsigned strength() const noexcept { return m_strength; }

    virtual string say() const noexcept override
    {
        return "AR" + string(m_strength, 'R') + "GH!";
    }

    DERIVED_FROM(animal);

private:
    unsigned m_strength;
};

// ���� ����� ��� ����, ������� �� ��������, �� ����� ���������.
class mushroom : public creature
{
public:
    mushroom(bool is_poisoned) noexcept
        : m_poisoned(is_poisoned) { }

    bool poisoned() const noexcept { return m_poisoned; }

    DERIVED_FROM(creature);

public:
    bool m_poisoned;
};

// ����������, �� ������������ ������� ��� ����� � �����!
string eat(const mushroom& M) noexcept
{
    return M.poisoned() ? "It was very bad idea!.." : "So tasty!!!";
}

// ���������!

int main()
{
    try
    {
        cout << boolalpha
             << "\n Time to test hierarchy of classes:"
             << "\n > animal is derived from creature: " << is_derived<animal>::from<creature>()
             << "\n > creature is derived from animal: " << is_derived<creature>::from<animal>()
             << "\n > cat is derived from animal: " << is_derived<cat>::from<animal>()
             << "\n > dog is derived from animal: " << is_derived<dog>::from<animal>()
             << "\n > cat is derived from creature: " << is_derived<cat>::from<creature>()
             << "\n > dog is derived from creature: " << is_derived<dog>::from<creature>()
             << "\n > creature is derived from dog: " << is_derived<creature>::from<dog>()
             << "\n > mushroom is derived from creature: " << is_derived<mushroom>::from<creature>()
             << "\n > mushroom is derived from animal: " << is_derived<mushroom>::from<animal>()
             << "\n > mushroom is derived from cat: " << is_derived<mushroom>::from<cat>()
             << "\n > dog is derived from mushroom: " << is_derived<dog>::from<mushroom>()
             << "\n > creature is derived from creature: " << is_derived<creature>::from<creature>()
             << "\n > mushroom is derived from mushroom: " << is_derived<mushroom>::from<mushroom>()
             << "\n > animal is derived from animal: " << is_derived<animal>::from<animal>()
             << "\n > cat is derived from cat: " << is_derived<cat>::from<cat>()
             << "\n > dog is derived from dog: " << is_derived<dog>::from<dog>()
             << endl;

        vector<shared_ptr<creature>> Zoo {
            make_shared<cat>("Teddy", true),
            make_shared<creature>(),
            make_shared<dog>("Sultan", 10),
            make_shared<cat>("Dusty", false),
            make_shared<mushroom>(false),
            make_shared<dog>("Winky", 2),
            make_shared<mushroom>(true)
        };

        cout << "\n Let's check the Zoo:";
        for (const shared_ptr<creature>& X : Zoo)
        {
            cout << "\n > " << X->who().name();

            if (X->is<animal>())
            {
                const animal& A = X->as<animal>();
                cout << " called \"" << A.name() << '"'
                     << " said: \"" << A.say() << '"';

                if (A.is<cat>())
                {
                    const cat& C = A.as<cat>();
                    cout << (C.cute() ? " So cute!" : " Not cute!");
                }
                else if (A.is<dog>())
                {
                    const dog& D = A.as<dog>();
                    cout << " Strength: " << D.strength();
                }
                else
                {
                    cout << " Nobody knows who is that beast!";
                }
            }
            else if (X->is<mushroom>())
            {
                const mushroom& M = X->as<mushroom>();
                cout << " - eat this! ... " << eat(M);
            }
            else
            {
                cout << " - unknown creature! Sensation!";
            }
        }
    }
    catch (exception& e)
    {
        cerr << "\n !> Unhandled exception: " << e.what() << endl;
        return -1;
    }
    catch (...)
    {
        cerr << "\n !> Unhandled exception of non-standard type was thrown." << endl;
        return -1;
    }

    cout << "\n\n -- Finished successfully --" << endl;
    return 0;
}

// �������������� �������.
// ��� ������ ������������ � ������������ ������� ����������!
// ����� �������, ��� ������������� ������ ���� �� ������
// ����������, �� ����� � ������������, ������� �������� �������.

const class_id& creature::id() noexcept
{
    static const class_id creature_id("creature");
    return creature_id;
}

const class_id& animal::id() noexcept
{
    static const class_id animal_id("animal");
    return animal_id;
}

const class_id& cat::id() noexcept
{
    static const class_id cat_id("cat");
    return cat_id;
}

const class_id& dog::id() noexcept
{
    static const class_id dog_id("dog");
    return dog_id;
}

const class_id& mushroom::id() noexcept
{
    static const class_id mushroom_id("mushroom");
    return mushroom_id;
}
