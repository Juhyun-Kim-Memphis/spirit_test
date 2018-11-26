#include <gtest/gtest.h>

#include <boost/bind.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/include/io.hpp>
#include <ostream>


namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;
namespace phoenix = boost::phoenix;

struct column_definition {
    std::string column;
    std::string datatype;
};

BOOST_FUSION_ADAPT_STRUCT(
    column_definition,
    (std::string, column)
    (std::string, datatype)
)

struct relational_properties {
    std::vector<column_definition> column_definitions;
};

BOOST_FUSION_ADAPT_STRUCT(
    relational_properties,
    (std::vector<column_definition>, column_definitions)
)

struct CreateTable {
    std::string table;
    relational_properties relational_properties_;
};

BOOST_FUSION_ADAPT_STRUCT(
    CreateTable,
    (std::string, table)
    (relational_properties, relational_properties_)
)

template <typename Iterator>
struct CreateTableGrammar
        : qi::grammar<Iterator, CreateTable(), ascii::space_type>
{
    CreateTableGrammar()
            : CreateTableGrammar::base_type(start, "CreateTable")
    {
        using qi::eps;
        using qi::lit;
        using qi::string;
        using qi::lexeme;
        using ascii::char_;
        using ascii::alnum;
        using qi::_val;
        using qi::_1;
        using boost::phoenix::push_back;

        start %= eps >> "CREATE" >> "TABLE" >> table >> ( relational_table /*| object_table | XMLType_table*/ );
        table %= name;
        relational_table %= eps >> -('(' >> relational_properties >> ')') >> ';';

        relational_properties %= (column >> datatype) % ',';
        column %= name;
        datatype %= name;
        name %= lexeme[+(alnum)];
    }

    qi::rule<Iterator, CreateTable(), ascii::space_type> start;
    qi::rule<Iterator, std::string(), ascii::space_type> table;

    qi::rule<Iterator, relational_properties(), ascii::space_type> relational_table;

    qi::rule<Iterator, std::vector<column_definition>, ascii::space_type> relational_properties;
    qi::rule<Iterator, std::string()> column;
    qi::rule<Iterator, std::string()> datatype;

    qi::rule<Iterator, std::string()> name;
};

TEST(Parser, CreateTable){
    CreateTable createTable;
    std::string inputText("CREATE TABLE t1 (c1 NUMBER, c2 VARCHAR, c3 DOUBLE);");

    using boost::spirit::ascii::space;
    typedef std::string::const_iterator iterator_type;
    CreateTableGrammar<iterator_type> grammar;

    std::string::const_iterator iter = inputText.begin();
    std::string::const_iterator end = inputText.end();
    bool r = phrase_parse(iter, end, grammar, space, createTable);

    if (r && iter == end)
    {
        std::cout << "-------------------------\n";
        std::cout << "Parsing succeeded\n";
        std::cout << "got: " << createTable.table << std::endl;

        for(auto &e : createTable.relational_properties_.column_definitions)
            std::cout<<"[T:"<< e.datatype <<", name:"<<e.column <<"]";
        std::cout<<std::endl;

        std::cout << "\n-------------------------\n";
    }
    else
    {
        std::cerr <<"iter::" << *iter<<std::endl;
        std::cout << "-------------------------\n";
        std::cout << "Parsing failed r:"<<r<<"\n";
        std::cout << "-------------------------\n";
    }
}

TEST(Parser, CreateTable2){
    CreateTable createTable;

}
