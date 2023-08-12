#include "DBUtil.hpp"

 DBUtil* DBUtil::instance;
 std::mutex DBUtil::mut;

DBUtil::DBUtil(string mongoURIStr, string dbName, string collName)
: dbName{dbName}, collName{collName}, mongoURI { mongocxx::uri{ mongoURIStr }} 
{
}

DBUtil::~DBUtil() 
{
}

void DBUtil::connect()
{
    mongocxx::instance inst{};
    mongocxx::options::client client_options;
    auto api = mongocxx::options::server_api{mongocxx::options::server_api::version::k_version_1};
    client_options.server_api_opts(api);
    conn = mongocxx::client{ mongoURI, client_options };
	vector<string> dbs = getDatabases(conn);
	if (!(std::find(dbs.begin(), dbs.end(), dbName) != dbs.end()))
        conn[dbName];
	database = conn.database(dbName);
    auto allCollections = database.list_collection_names();
	if (!(std::find(allCollections.begin(), allCollections.end(), collName) != allCollections.end()))
		createCollection(database, collName);
	collection = database.collection(collName);
}

void DBUtil::print() 
{
    printCollection(collection);
}

int DBUtil::getLogs(cQueue& q) {
	if (collection.count_documents({}) == 0)
	{
		cout << "Collection is empty.\nFILE: " << __FILE__ << "\nLINE: " << __LINE__;
		return -1;
	}
	auto cursor = collection.find({});
	int size = 0;
	for (auto&& doc : cursor) {
		q.enqueue(string(doc["log"].get_string().value));
		size++;
	}
	return size;
}

void DBUtil::insertLogs(vector<string> logs) {
    for (auto log : logs) 
        insertDocument(createDocument({ {"log", log} }));
}

DBUtil *DBUtil::getInstance(string mongoURIStr,
            				string dbName,
							string collName)
{
	std::lock_guard<std::mutex> lock(DBUtil::mut);
    if (instance == nullptr) {
		instance = new DBUtil(mongoURIStr, dbName, collName);
		instance->connect();
	}
    return instance;
}

inline void DBUtil::createCollection(mongocxx::database& db, const string& collectionName) const
{
	db.create_collection(collectionName);
}

inline vector<string> DBUtil::getDatabases(mongocxx::client &client) const 
{
    return client.list_database_names();
}

void DBUtil::printCollection(mongocxx::collection& collection)
{
	if (collection.count_documents({}) == 0)
	{
		cout << "Collection is empty.\n";
		return;
	}
	auto cursor = collection.find({});
	for (auto&& doc : cursor)
		cout << bsoncxx::to_json(doc) << "\n";
}

bsoncxx::document::value DBUtil::createDocument(const vector<pair<string, string>>& keyValues)
{
	bsoncxx::builder::stream::document document{};
	for (auto& keyValue : keyValues)
		document << keyValue.first << keyValue.second;
	return document << bsoncxx::builder::stream::finalize;
}


inline void DBUtil::insertDocument(const bsoncxx::document::value& document) 
{
    collection.insert_one(document.view());
}

