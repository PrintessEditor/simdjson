#include "simdjson.h"
#include "test_ondemand.h"

using namespace std;
using namespace simdjson;
using error_code=simdjson::error_code;

#if SIMDJSON_EXCEPTIONS

bool basics_1() {
  TEST_START();

  ondemand::parser parser;
  auto json = padded_string::load("twitter.json");
  ondemand::document doc = parser.iterate(json); // load and parse a file

  simdjson_unused auto unused_doc = doc.get_object();

  TEST_SUCCEED();
}

bool basics_2() {
  TEST_START();

  ondemand::parser parser;
  auto json = "[1,2,3]"_padded; // The _padded suffix creates a simdjson::padded_string instance
  ondemand::document doc = parser.iterate(json); // parse a string

  simdjson_unused auto unused_doc = doc.get_array();

  TEST_SUCCEED();
}

bool basics_3() {
  TEST_START();

  ondemand::parser parser;
  char json[3+SIMDJSON_PADDING];
  strcpy(json, "[1]");
  ondemand::document doc = parser.iterate(json, strlen(json), sizeof(json));

  simdjson_unused auto unused_doc = doc.get_array();

  TEST_SUCCEED();
}


bool json_array_with_array_count() {
  TEST_START();
  ondemand::parser parser;
  auto cars_json = R"( [ 40.1, 39.9, 37.7, 40.4 ] )"_padded;
  auto doc = parser.iterate(cars_json);
  auto arr = doc.get_array();
  size_t count = arr.count_elements();
  ASSERT_EQUAL(4, count);
  std::cout << count << std::endl;
  // We deliberately do it twice:
  count = arr.count_elements();
  ASSERT_EQUAL(4, count);
  std::cout << count << std::endl;
  // Next, we check whether we can iterate normally:
  std::vector<double> values(count);
  size_t index = 0;
  for(double x : arr) { values[index++] = x; }
  ASSERT_EQUAL(index, count);
  TEST_SUCCEED();
}

bool json_value_with_array_count() {
  TEST_START();
  ondemand::parser parser;
  auto cars_json = R"( {"array":[ 40.1, 39.9, 37.7, 40.4 ]} )"_padded;
  auto doc = parser.iterate(cars_json);
  auto val = doc["array"];
  size_t count = val.count_elements();
  ASSERT_EQUAL(4, count);
  std::cout << count << std::endl;
  // We deliberately do it twice:
  count = val.count_elements();
  ASSERT_EQUAL(4, count);
  std::cout << count << std::endl;
  std::vector<double> values(count);
  // Next, we check whether we can iterate normally:
  size_t index = 0;
  for(double x : val) { values[index++] = x; }
  ASSERT_EQUAL(index, count);
  TEST_SUCCEED();
}


bool json_array_count() {
  TEST_START();
  ondemand::parser parser;
  auto cars_json = R"( [ 40.1, 39.9, 37.7, 40.4 ] )"_padded;
  auto doc = parser.iterate(cars_json);
  size_t count = doc.count_elements();
  ASSERT_EQUAL(4, count);
  std::cout << count << std::endl;
  // We deliberately do it twice:
  count = doc.count_elements();
  ASSERT_EQUAL(4, count);
  std::cout << count << std::endl;
  std::vector<double> values(count);
  size_t index = 0;
  for(double x : doc) { values[index++] = x; }
  ASSERT_EQUAL(index, count);
  TEST_SUCCEED();
}

bool json_array_count_complex() {
  TEST_START();
  ondemand::parser parser;
  auto cars_json = R"( { "test":[ { "val1":1, "val2":2 }, { "val1":1, "val2":2 }, { "val1":1, "val2":2 } ] }   )"_padded;
  auto doc = parser.iterate(cars_json);
  auto test_array = doc.find_field("test").get_array();
  size_t count = test_array.count_elements();
  std::cout << "Number of elements: " <<  count << std::endl;
  size_t c = 0;
  for(ondemand::object elem : test_array) {
    std::cout << simdjson::to_json_string(elem);
    c++;
  }
  std::cout << std::endl;
  ASSERT_EQUAL(c, count);
  TEST_SUCCEED();

}

bool using_the_parsed_json_1() {
  TEST_START();

  try {

    ondemand::parser parser;
    auto json = R"(  { "x": 1, "y": 2 }  )"_padded;
    auto doc = parser.iterate(json);
    double y = doc.find_field("y"); // The cursor is now after the 2 (at })
    double x = doc.find_field("x"); // This fails, because there are no more fields after "y"

    cout << x << ", " << y << endl;
  } catch (...) {
    TEST_SUCCEED();
  }
  TEST_FAIL("expected an exception");
}

bool using_the_parsed_json_2() {
  TEST_START();

  ondemand::parser parser;
  auto json = R"(  { "x": 1, "y": 2 }  )"_padded;
  auto doc = parser.iterate(json);
  double y = doc["y"]; // The cursor is now after the 2 (at })
  double x = doc["x"]; // Success: [] loops back around to find "x"

  cout << x << ", " << y << endl;

  TEST_SUCCEED();
}

  bool big_integer() {
    TEST_START();
    simdjson::ondemand::parser parser;
    simdjson::padded_string docdata =  R"({"value":12321323213213213213213213213211223})"_padded;
    simdjson::ondemand::document doc = parser.iterate(docdata);
    simdjson::ondemand::object obj = doc.get_object();
    string_view token = obj["value"].raw_json_token();
    std::cout << token << std::endl;
    // token == "12321323213213213213213213213211223"
    TEST_SUCCEED();
  }

  bool big_integer_in_string() {
    TEST_START();
    simdjson::ondemand::parser parser;
    simdjson::padded_string docdata =  R"({"value":"12321323213213213213213213213211223"})"_padded;
    simdjson::ondemand::document doc = parser.iterate(docdata);
    simdjson::ondemand::object obj = doc.get_object();
    string_view token = obj["value"].raw_json_token();
    std::cout << token << std::endl;
    // token == "\"12321323213213213213213213213211223\""
    TEST_SUCCEED();
  }
bool using_the_parsed_json_3() {
  TEST_START();

  ondemand::parser parser;
  auto cars_json = R"( [
    { "make": "Toyota", "model": "Camry",  "year": 2018, "tire_pressure": [ 40.1, 39.9, 37.7, 40.4 ] },
    { "make": "Kia",    "model": "Soul",   "year": 2012, "tire_pressure": [ 30.1, 31.0, 28.6, 28.7 ] },
    { "make": "Toyota", "model": "Tercel", "year": 1999, "tire_pressure": [ 29.8, 30.0, 30.2, 30.5 ] }
  ] )"_padded;

  // Iterating through an array of objects
  for (ondemand::object car : parser.iterate(cars_json)) {
    // Accessing a field by name
    cout << "Make/Model: " << std::string_view(car["make"]) << "/" << std::string_view(car["model"]) << endl;

    // Casting a JSON element to an integer
    uint64_t year = car["year"];
    cout << "- This car is " << 2020 - year << " years old." << endl;

    // Iterating through an array of floats
    double total_tire_pressure = 0;
    for (double tire_pressure : car["tire_pressure"]) {
      total_tire_pressure += tire_pressure;
    }
    cout << "- Average tire pressure: " << (total_tire_pressure / 4) << endl;
  }

  TEST_SUCCEED();
}


bool using_the_parsed_json_rewind() {
  TEST_START();

  ondemand::parser parser;
  auto cars_json = R"( [
    { "make": "Toyota", "model": "Camry",  "year": 2018, "tire_pressure": [ 40.1, 39.9, 37.7, 40.4 ] },
    { "make": "Kia",    "model": "Soul",   "year": 2012, "tire_pressure": [ 30.1, 31.0, 28.6, 28.7 ] },
    { "make": "Toyota", "model": "Tercel", "year": 1999, "tire_pressure": [ 29.8, 30.0, 30.2, 30.5 ] }
  ] )"_padded;

  auto doc = parser.iterate(cars_json);
  size_t count = 0;
  for (simdjson_unused ondemand::object car : doc) {
    if(car["make"] == "Toyota") { count++; }
  }
  std::cout << "We have " << count << " Toyota cars.\n";
  doc.rewind();
  for (ondemand::object car : doc) {
    cout << "Make/Model: " << std::string_view(car["make"]) << "/" << std::string_view(car["model"]) << endl;
  }
  TEST_SUCCEED();
}

bool using_the_parsed_json_4() {
  TEST_START();

  ondemand::parser parser;
  auto points_json = R"( [
      {  "12345" : {"x":12.34, "y":56.78, "z": 9998877}   },
      {  "12545" : {"x":11.44, "y":12.78, "z": 11111111}  }
    ] )"_padded;

  // Parse and iterate through an array of objects
  for (ondemand::object points : parser.iterate(points_json)) {
    // Iterating through an object, you iterate through key-value pairs (a 'field').
    for (auto point : points) {
      // Get the key corresponding the the field 'point'.
      cout << "id: " << std::string_view(point.unescaped_key()) << ": (";
      // Get the value corresponding the the field 'point'.
      ondemand::object xyz = point.value();
      cout << xyz["x"].get_double() << ", ";
      cout << xyz["y"].get_double() << ", ";
      cout << xyz["z"].get_int64() << endl;
    }
  }

  TEST_SUCCEED();
}

bool using_the_parsed_json_5() {
  TEST_START();

  auto abstract_json = R"(
    { "str" : { "123" : {"abc" : 3.14 } } }
  )"_padded;
  ondemand::parser parser;
  auto doc = parser.iterate(abstract_json);
  cout << doc["str"]["123"]["abc"].get_double() << endl; // Prints 3.14

  TEST_SUCCEED();
}

#endif // SIMDJSON_EXCEPTIONS



bool using_the_parsed_json_no_exceptions() {
  TEST_START();

  ondemand::parser parser;
  auto cars_json = R"( [
    { "make": "Toyota", "model": "Camry",  "year": 2018, "tire_pressure": [ 40.1, 39.9, 37.7, 40.4 ] },
    { "make": "Kia",    "model": "Soul",   "year": 2012, "tire_pressure": [ 30.1, 31.0, 28.6, 28.7 ] },
    { "make": "Toyota", "model": "Tercel", "year": 1999, "tire_pressure": [ 29.8, 30.0, 30.2, 30.5 ] }
  ] )"_padded;
  ondemand::document doc;

  // Iterating through an array of objects
  auto error = parser.iterate(cars_json).get(doc);
  if(error) { std::cerr << error << std::endl; return false; }
  ondemand::array cars;
  error = doc.get_array().get(cars);

  for (auto car_value : cars) {
    ondemand::object car;
    error = car_value.get_object().get(car);
    if(error) { std::cerr << error << std::endl; return false; }

    // Accessing a field by name
    std::string_view make;
    std::string_view model;
    error = car["make"].get(make);
    if(error) { std::cerr << error << std::endl; return false; }
    error = car["model"].get(model);
    if(error) { std::cerr << error << std::endl; return false; }

    cout << "Make/Model: " << make << "/" << model << endl;

    // Casting a JSON element to an integer
    uint64_t year;
    error = car["year"].get(year);
    if(error) { std::cerr << error << std::endl; return false; }
    cout << "- This car is " << 2020 - year << " years old." << endl;

    // Iterating through an array of floats
    double total_tire_pressure = 0;
    ondemand::array pressures;
    error = car["tire_pressure"].get_array().get(pressures);
    if(error) { std::cerr << error << std::endl; return false; }
    for (auto tire_pressure_value : pressures) {
      double tire_pressure;
      error = tire_pressure_value.get_double().get(tire_pressure);
      if(error) { std::cerr << error << std::endl; return false; }
      total_tire_pressure += tire_pressure;
    }
    cout << "- Average tire pressure: " << (total_tire_pressure / 4) << endl;
  }
  TEST_SUCCEED();
}
int using_the_parsed_json_6_process() {
  auto abstract_json = R"(
    { "str" : { "123" : {"abc" : 3.14 } } }
  )"_padded;
  ondemand::parser parser;

  double value;
  auto doc = parser.iterate(abstract_json);
  auto error = doc["str"]["123"]["abc"].get(value);
  if (error) { std::cerr << error << std::endl; return EXIT_FAILURE; }
  cout << value << endl; // Prints 3.14

  return EXIT_SUCCESS;
}
bool using_the_parsed_json_6() {
  TEST_START();
  ASSERT_EQUAL(using_the_parsed_json_6_process(), EXIT_SUCCESS);
  TEST_SUCCEED();
}

const padded_string cars_json = R"( [
  { "make": "Toyota", "model": "Camry",  "year": 2018, "tire_pressure": [ 40.1, 39.9, 37.7, 40.4 ] },
  { "make": "Kia",    "model": "Soul",   "year": 2012, "tire_pressure": [ 30.1, 31.0, 28.6, 28.7 ] },
  { "make": "Toyota", "model": "Tercel", "year": 1999, "tire_pressure": [ 29.8, 30.0, 30.2, 30.5 ] }
] )"_padded;

bool json_pointer_simple() {
    TEST_START();
    ondemand::parser parser;
    ondemand::document cars;
    double x;
    ASSERT_SUCCESS(parser.iterate(cars_json).get(cars));
    ASSERT_SUCCESS(cars.at_pointer("/0/tire_pressure/1").get(x));
    ASSERT_EQUAL(x,39.9);
    TEST_SUCCEED();
}

bool json_pointer_multiple() {
	TEST_START();
	ondemand::parser parser;
	ondemand::document cars;
	size_t size;
	ASSERT_SUCCESS(parser.iterate(cars_json).get(cars));
	ASSERT_SUCCESS(cars.count_elements().get(size));
	double expected[] = {39.9, 31, 30};
	for (size_t i = 0; i < size; i++) {
		std::string json_pointer = "/" + std::to_string(i) + "/tire_pressure/1";
		double x;
		ASSERT_SUCCESS(cars.at_pointer(json_pointer).get(x));
		ASSERT_EQUAL(x,expected[i]);
	}
	TEST_SUCCEED();
}

bool json_pointer_rewind() {
  TEST_START();
  auto json = R"( {
  "k0": 27,
  "k1": [13,26],
  "k2": true
  } )"_padded;

  ondemand::parser parser;
  ondemand::document doc;
  uint64_t i;
  bool b;
  ASSERT_SUCCESS(parser.iterate(json).get(doc));
  ASSERT_SUCCESS(doc.at_pointer("/k1/1").get(i));
  ASSERT_EQUAL(i,26);
  ASSERT_SUCCESS(doc.at_pointer("/k2").get(b));
  ASSERT_EQUAL(b,true);
  doc.rewind();	// Need to manually rewind to be able to use find_field properly from start of document
  ASSERT_SUCCESS(doc.find_field("k0").get(i));
  ASSERT_EQUAL(i,27);
  TEST_SUCCEED();
}

bool iterate_many_example() {
  TEST_START();
  auto json = R"([1,2,3]  {"1":1,"2":3,"4":4} [1,2,3]  )"_padded;
  simdjson::ondemand::parser parser;
  simdjson::ondemand::document_stream stream;
  ASSERT_SUCCESS(parser.iterate_many(json).get(stream));
  auto i = stream.begin();
  size_t count{0};
  size_t expected_indexes[3] = {0,9,29};
  std::string_view expected_doc[3] = {"[1,2,3]", R"({"1":1,"2":3,"4":4})", "[1,2,3]"};
  for(; i != stream.end(); ++i) {
      auto doc = *i;
      ASSERT_SUCCESS(doc.type());
      ASSERT_SUCCESS(i.error());
      ASSERT_EQUAL(i.current_index(),expected_indexes[count]);
      ASSERT_EQUAL(i.source(),expected_doc[count]);
      count++;
  }
  TEST_SUCCEED();
}

std::string my_string(ondemand::document& doc) {
  std::stringstream ss;
  ss << doc;
  return ss.str();
}

bool iterate_many_truncated_example() {
  TEST_START();
  auto json = R"([1,2,3]  {"1":1,"2":3,"4":4} {"key":"intentionally unclosed string  )"_padded;
  simdjson::ondemand::parser parser;
  simdjson::ondemand::document_stream stream;
  ASSERT_SUCCESS( parser.iterate_many(json,json.size()).get(stream) );
  std::string_view expected[2] = {"[1,2,3]", R"({"1":1,"2":3,"4":4})"};
  size_t count{0};
  for(auto i = stream.begin(); i != stream.end(); ++i) {
      ASSERT_EQUAL(i.source(),expected[count++]);
  }
  size_t truncated = stream.truncated_bytes();
  ASSERT_EQUAL(truncated,39);
  TEST_SUCCEED();
}

bool ndjson_basics_example() {
  TEST_START();
  auto json = R"({ "foo": 1 } { "foo": 2 } { "foo": 3 } )"_padded;
  ondemand::parser parser;
  ondemand::document_stream docs;
  ASSERT_SUCCESS( parser.iterate_many(json).get(docs) );
  size_t count{0};
  int64_t expected[3] = {1,2,3};
  for (auto doc : docs) {
    int64_t actual;
    ASSERT_SUCCESS( doc["foo"].get(actual) );
    ASSERT_EQUAL( actual,expected[count++] );
  }
  TEST_SUCCEED();
}
bool stream_capacity_example() {
  auto json = R"([1,2,3,4,5] [1,2,3,4,5] [1,2,3,4,5] [1,2,3,4,5] [1,2,3,4,5] [1,2,3,4,5] [1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100])"_padded;
  ondemand::parser parser;
  ondemand::document_stream stream;
  size_t counter{0};
  auto error = parser.iterate_many(json, 50).get(stream);
  if( error ) { /* handle the error */ }
  for (auto doc: stream) {
    if(counter < 6) {
      int64_t val;
      error = doc.at_pointer("/4").get(val);
      if( error ) { /* handle the error */ }
      std::cout << "5 = " << val << std::endl;
    } else {
      ondemand::value val;
      error = doc.at_pointer("/4").get(val);
      // error == simdjson::CAPACITY
      if(error) {
        std::cerr << error << std::endl;
        // We left 293 bytes unprocessed at the tail end of the input.
        std::cout << " unprocessed bytes at the end: " << stream.truncated_bytes() << std::endl;
        break;
      }
    }
    counter++;
  }
  return true;
}



bool simple_error_example() {
    ondemand::parser parser;
    auto json = R"({"bad number":3.14.1 })"_padded;
    ondemand::document doc;
    if( parser.iterate(json).get(doc) != SUCCESS ) { return false; }
    double x;
    auto error = doc["bad number"].get_double().get(x);
    // returns "simdjson::NUMBER_ERROR"
    if(error != SUCCESS) {
      std::cout << error << std::endl;
      return false;
    }
    std::cout << "Got " << x << std::endl;
    return true;
}


#if SIMDJSON_EXCEPTIONS
  bool simple_error_example_except() {
    TEST_START();
    ondemand::parser parser;
    auto json = R"({"bad number":3.14.1 })"_padded;
    try {
      ondemand::document doc = parser.iterate(json);
      double x = doc["bad number"].get_double();
      std::cout << "Got " << x << std::endl;
      return true;
    } catch(simdjson_error& e) {
      // e.error() == NUMBER_ERROR
      std::cout << e.error() << std::endl;
      return false;
    }
  }
#endif

int load_example() {
  simdjson::ondemand::parser parser;
  simdjson::ondemand::document tweets;
  padded_string json;
  auto error = padded_string::load("twitter.json").get(json);
  if(error) { std::cerr << error << std::endl; return EXIT_FAILURE; }
  error = parser.iterate(json).get(tweets);
  if(error) { std::cerr << error << std::endl; return EXIT_FAILURE; }
  uint64_t identifier;
  error = tweets["statuses"].at(0)["id"].get(identifier);
  if(error) { std::cerr << error << std::endl; return EXIT_FAILURE; }
  std::cout << identifier << std::endl;
  return EXIT_SUCCESS;
}


int example_1() {
  TEST_START();
  simdjson::ondemand::parser parser;
  //auto error = padded_string::load("twitter.json").get(json);
  // if(error) { std::cerr << error << std::endl; return EXIT_FAILURE; }
  padded_string json = R"( {
  "statuses": [
    {
      "id": 505874924095815700
    },
    {
      "id": 505874922023837700
    }
  ],
  "search_metadata": {
    "count": 100
  }
} )"_padded;

  simdjson::ondemand::document tweets;
  auto error = parser.iterate(json).get(tweets);
  if( error ) { return EXIT_FAILURE; }
  simdjson::ondemand::value res;
  error = tweets["search_metadata"]["count"].get(res);
  if (error != SUCCESS) {
    std::cerr << "could not access keys" << std::endl;
    return EXIT_FAILURE;
  }
  std::cout << res << " results." << std::endl;
  return true;
}
#if SIMDJSON_EXCEPTIONS
int load_example_except() {
  simdjson::ondemand::parser parser;
  padded_string json = padded_string::load("twitter.json");
  simdjson::ondemand::document tweets = parser.iterate(json);
  uint64_t identifier = tweets["statuses"].at(0)["id"];
  std::cout << identifier << std::endl;
  return EXIT_SUCCESS;
}
#endif
bool test_load_example() {
  TEST_START();
  simdjson::ondemand::parser parser;
  simdjson::ondemand::document tweets;
  padded_string json = R"( {"statuses":[{"id":1234}]} )"_padded;
  auto error = parser.iterate(json).get(tweets);
  if(error) { std::cerr << error << std::endl; return false; }
  uint64_t identifier;
  error = tweets["statuses"].at(0)["id"].get(identifier);
  if(error) { std::cerr << error << std::endl; return false; }
  std::cout << identifier << std::endl;
  return identifier == 1234;
}
int main() {
  if (
    true
#if SIMDJSON_EXCEPTIONS
//    && basics_1() // Fails because twitter.json isn't in current directory. Compile test only.
    && json_value_with_array_count()
    && json_array_with_array_count()
    && json_array_count_complex()
    && json_array_count()
    && using_the_parsed_json_rewind()
    && basics_2()
    && using_the_parsed_json_1()
    && using_the_parsed_json_2()
    && big_integer()
    && big_integer_in_string()
    && using_the_parsed_json_3()
    && using_the_parsed_json_4()
    && using_the_parsed_json_5()
#endif
    && using_the_parsed_json_6()
    && json_pointer_simple()
    && json_pointer_multiple()
    && json_pointer_rewind()
    && iterate_many_example()
    && iterate_many_truncated_example()
    && ndjson_basics_example()
    && stream_capacity_example()
    && test_load_example()
    && example_1()
    && using_the_parsed_json_no_exceptions()
  ) {
    return 0;
  } else {
    return 1;
  }
}
