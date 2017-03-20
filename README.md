# FixedJSONParserCPP

[![Build Status](https://travis-ci.org/Grauniad/FixedJSONParserCPP.svg?branch=master)](https://travis-ci.org/Grauniad/FixedJSONParserCPP)
[![Coverage Status](https://coveralls.io/repos/github/Grauniad/FixedJSONParserCPP/badge.svg?branch=master)](https://coveralls.io/github/Grauniad/FixedJSONParserCPP?branch=master)

## Parse JSON Messages into native types:
```c++
/*
 * Request the creation of a new order
 *
 * REQUEST:
 *    {
 *        account:      "Account ID",
 *        exchange:     "Exchange ID",
 *        stock:        "Symbol",
 *        type:         "limit",
 *        direction:    "buy",
 *        quantity:     100,
 *        limit_price:  100
 *    }
 */
 // Define the request message
 namespace {
    NewStringField(account);
    NewStringField(direction);
    NewStringField(exchange);
    NewI64Field(limit_price);
    NewI64Field(quantity);
    NewStringField(stock);
    NewStringField(type);
    typedef SimpleParsedJSON<
         account,
         direction,
         exchange,
         limit_price,
         quantity,
         stock,
         type
    > Request;
    Request request;
}

void ReqNewOrder::Setup(const char* req, OrderDefinition& order) {
    std::string error;
    request.Clear();
    if ( !request.Parse(req,error)) {
        throw InvalidRequestException{0,error};
    }

    order.account = request.Get<account>();
    order.direction.SetValue(request.Get<direction>());
    order.venue = request.Get<exchange>();
    order.limitPrice = request.Get<limit_price>();
    order.originalQuantity = request.Get<quantity>();
    order.symbol = request.Get<stock>();
    order.type.SetValue(request.Get<type>());
}
````
## Create JSON Messages from native types:
```c++
/*
 *
 * REPLY:
 *    {
 *        fileName:  "gather.out",
 *        units: ["Ir", "Dr"],
 *    }
 *
 *   // The type is defined in the request handler class to be:
 *   NewStringField(fileName);
 *   NewStringArrayField(units);
 *   NewStringArrayField(path);
 *   SimpleParsedJSON<fileName,units,path> reply;
 */
   
    // Build up the reply in the handler...
    std::string ReqGetGraphDetails::OnRequest(const char* JSON) {
        reply.Clear();

        reply.Get<fileName>() = parent->FileName();
        LookupUnits();
        
        // Convert the stored native types to a valid JSON object...
        return reply.GetJSONString();
    }
    
    // Array types are stored in a std::vector...
    void ReqGetGraphDetails::LookupUnits() {
        for ( const size_t& i : NodeConfig::Instance().DisplayIdxs() ) {
            reply.Get<units>().push_back(GetUnit(i));
        }
    }
 ```
