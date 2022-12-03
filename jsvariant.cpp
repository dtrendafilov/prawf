#include <variant>
#include <memory>
#include <vector>
#include <string>

class JSValue;

typedef std::unique_ptr<JSValue> JSValueUnique;
typedef std::shared_ptr<JSValue> JSValueShared;
typedef std::vector<JSValue> JSValueVector;

/* typedef std::variant<bool, double, std::string, JSValueUnique, JSValueShared, */
/*         JSValueVector> JSValue; */

using JSValue =  std::variant<bool, double, std::string, JSValueUnique, JSValueShared,
        JSValueVector>;

int main()
{
    return 0;
}


