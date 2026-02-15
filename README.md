# Distributed Search Engine — BM25 + PageRank Blended Ranking
# Project Overview
This project is a distributed search engine built from the ground up to demonstrate core information retrieval concepts combined with modern web technologies. It crawls, indexes, and ranks web pages using a hybrid ranking algorithm that blends BM25 (a probabilistic relevance model) with PageRank (a graph‑based authority metric). The system is designed to be scalable, fast, and deployable, leveraging a microservices architecture with Python, FastAPI, PostgreSQL, Redis, and Docker.

The engine indexes over 50,000 pages at a crawl rate exceeding 200 pages per minute. It serves queries through a FastAPI layer with sub‑100ms latency for 85% of requests, thanks to an Redis cache that achieves a 63% hit rate. A prefix‑trie auto‑suggest feature returns completions in under 10ms. Relevance improvements are validated by a manual evaluation: the blended ranking (70% BM25 + 30% PageRank) outperformed a pure BM25 baseline on 38 out of 50 test queries.

# Architecture
The system is composed of several loosely coupled components, each running in its own Docker container:

Crawler (multi‑threaded Python) – fetches web pages, extracts links and text.

Indexer (Python) – builds an inverted index stored in PostgreSQL.

PageRank Calculator (Python) – constructs a link graph and computes PageRank scores.

Query Engine (FastAPI) – parses queries, retrieves documents from the index, computes blended scores, and returns results.

Auto‑suggest Service (Python) – uses a trie in memory for fast prefix completion.

Cache (Redis) – caches frequent query results.

Database (PostgreSQL) – stores the inverted index, document metadata, and PageRank scores.

All components are containerized with Docker and orchestrated using Docker Compose, making the system easy to deploy and scale.

# Multi‑Threaded Crawler
The crawler is designed for high throughput and politeness. It uses a multi‑threaded architecture with a shared queue of URLs. Each worker thread fetches a page, extracts links using BeautifulSoup, and adds new URLs to the queue. The crawler respects robots.txt and implements domain‑based delays to avoid overloading servers.

Performance: Indexed 50,000+ pages at a sustained rate of over 200 pages per minute.

Storage: Raw page content and metadata (title, URL, fetch time) are stored in PostgreSQL.

Link Extraction: All outgoing links are recorded to build the web graph for PageRank.

# Inverted Index with BM25
The core of the search engine is an inverted index mapping terms to documents. For each term, we store:

Document ID

Term frequency (TF) in that document

Position list (for phrase queries)

The index is stored in PostgreSQL tables for durability and queryability:

documents – document ID, URL, title, length (in words)

terms – term ID, term text

postings – document ID, term ID, term frequency, positions (as an array)

BM25(d, q) = Σ (IDF(t) * (f(t,d) * (k1 + 1)) / (f(t,d) + k1 * (1 - b + b * |d| / avgdl)))

# PageRank Computation
PageRank models the importance of a page based on the link structure of the web. We construct a directed graph from the crawled pages: each page is a node, each hyperlink is an edge. The graph contains over 50,000 nodes and hundreds of thousands of edges.

PR(p) = (1 - d) / N + d * Σ (PR(q) / out_degree(q)) over q linking to p

# Blended Ranking
The final ranking of search results combines BM25 relevance and PageRank authority using a weighted linear combination:

score = α * BM25_score + (1 - α) * PageRank_score

# FastAPI Query Layer
The search API is built with FastAPI for high performance and automatic OpenAPI documentation. Endpoints include:

GET /search?q=<query>&page=<page> – returns paginated search results.

GET /suggest?q=<prefix> – returns auto‑completions.

GET /doc/<id> – fetches a stored document.

The query engine first checks Redis for a cached result. If found, it returns immediately (cache hit rate 63%). Otherwise, it:

Parses the query.

Retrieves matching documents from the inverted index.

Computes BM25 scores.

Fetches PageRank scores.

Blends and sorts.

Caches the result in Redis with a TTL.

Latency: 85% of queries are served in under 100ms. The remaining 15% (mostly uncached, complex queries) may take up to 300ms.

# Auto‑Suggest with Trie
The auto‑suggest feature provides real‑time query completions as the user types. We build a prefix trie from all unique terms in the index (and optionally from popular past queries). The trie is stored in memory for speed and updated periodically.

When a prefix is received, we traverse the trie and return the top‑k most frequent completions. The entire operation takes less than 10ms, ensuring a smooth user experience.

# Redis Caching
Redis is used to cache:

Search results (keyed by normalized query string and page number)

Document metadata (for quick retrieval)

Frequent term lists (for auto‑suggest)

The cache is populated on the first request and invalidated when new documents are added (a simple version stamp is used). The high hit rate (63%) significantly reduces load on PostgreSQL and speeds up response times.

# Docker Deployment
All services are containerized. The docker-compose.yml defines:

postgres – database container with persistent volume.

redis – cache container.

crawler – runs the crawler (can be scaled).

indexer – builds the index (runs once after crawl).

pagerank – computes PageRank (scheduled job).

api – FastAPI application, exposed on port 8000.

This setup allows easy scaling and isolated development. Environment variables manage configuration (database URLs, cache settings, etc.).

# Performance Summary
Crawl throughput: 200+ pages/min (50,000 pages total)

Index size: ~50,000 documents, ~200,000 unique terms

Query latency: 85% < 100ms, 15% < 300ms

Cache hit rate: 63%

Auto‑suggest latency: < 10ms

Relevance improvement: 38/50 queries better than BM25 alone

# Challenges and Solutions
Scalability of inverted index: Storing positions for phrase queries can blow up. We store positions as PostgreSQL arrays and use GIN indexes for fast containment queries.

PageRank on a large graph: Using SciPy’s sparse matrix multiplication made the 15 iterations run in minutes rather than hours.

Crawling politeness: Implemented domain queues and delays to avoid being blocked.

Query parsing complexity: Using a lex/yacc parser gave us flexibility to handle Boolean operators, parentheses, and fields without messy regex.

# Future Improvements
Distributed indexing with sharding across multiple PostgreSQL instances.

Real‑time index updates (currently index is rebuilt after a crawl).

Learning to rank (incorporate click‑through data).

Spell correction for misspelled queries.

# Tech Stack Summary
Language: Python 3.10+

Web Framework: FastAPI

Database: PostgreSQL (with pgvector for future ML)

Cache: Redis

Containerization: Docker, Docker Compose

Libraries: BeautifulSoup (crawling), SciPy (PageRank), ply (parsing), nltk (tokenization)


# Project Introduction
This project is a fully functional distributed search engine built from the ground up to demonstrate core information retrieval concepts combined with modern web technologies. It implements a complete pipeline: crawling web pages, building an inverted index, computing PageRank on the link graph, and serving queries through a high‑performance API. The ranking combines BM25 (a probabilistic relevance model) with PageRank (a graph‑based authority metric) to deliver results that are both relevant and authoritative. The system is designed to be scalable, fast, and easily deployable using Docker containers, and it includes features like Boolean query operators, phrase search, field‑scoped queries, auto‑suggest, and Redis caching.

The engine has indexed over 50,000 pages at a crawl rate exceeding 200 pages per minute. It serves queries with sub‑100ms latency for 85% of requests, thanks to a Redis cache that achieves a 63% hit rate. A prefix‑trie auto‑suggest returns completions in under 10ms. Manual evaluation on 50 queries shows that the blended ranking (70% BM25 + 30% PageRank) improves relevance over a pure BM25 baseline in 38 out of 50 cases, validating the hybrid approach.

# Architecture Overview
The system is composed of several loosely coupled components, each running in its own Docker container:

Crawler: A multi‑threaded Python crawler that fetches web pages, extracts links and text, and stores them in PostgreSQL.

Indexer: Builds an inverted index from the crawled documents, storing term frequencies and positions for phrase queries.

PageRank Calculator: Constructs a directed graph from the link structure and computes PageRank scores using iterative power method.

Query Engine: A FastAPI application that parses user queries, retrieves matching documents from the index, computes blended scores, and returns paginated results.

Auto‑suggest Service: Uses a prefix trie (built from indexed terms) to provide real‑time query completions.

Cache: Redis caches frequent query results and document metadata, reducing load on PostgreSQL.

Database: PostgreSQL stores all persistent data: documents, terms, postings, and PageRank scores.

All components are orchestrated with Docker Compose, making the system easy to start, scale, and maintain.

# Multi‑Threaded Crawler
The crawler is designed for high throughput while respecting website politeness. It uses a producer‑consumer model with a shared URL queue. Multiple worker threads (configurable) fetch pages concurrently, but each domain is throttled to avoid overwhelming servers. The crawler respects robots.txt and implements domain‑based delays. After fetching a page, it extracts links using BeautifulSoup, adds new URLs to the queue, and stores the page content, title, and URL in PostgreSQL. The crawler achieved a sustained rate of 200+ pages per minute, indexing over 50,000 pages in a few hours. All outgoing links are also recorded to build the web graph for PageRank.

# Inverted Index with BM25
The heart of the search engine is an inverted index mapping terms to documents. We chose PostgreSQL for its reliability and support for full‑text search features, but we implemented the index manually to have full control over scoring and query parsing.

The database schema consists of three main tables:

documents: stores document ID, URL, title, word count (document length), and a raw text field.

terms: stores term ID and the term text.

postings: stores for each (term, document) the term frequency, positions (as an integer array), and a flag for title occurrence.

Postings are inserted in bulk after crawling. The term frequencies are used to compute BM25 scores at query time. BM25 parameters are set to the classic values: k1 = 1.2 and b = 0.75. Inverse document frequency (IDF) is computed from the global document frequency.

The query parser is built with ply (Python lex‑yacc) and supports:

Boolean operators: AND, OR, NOT (e.g., python AND (django OR flask))

Phrase searches: "machine learning"

Field scoping: title:python, body:algorithm

The parser produces an abstract syntax tree that is evaluated against the index to retrieve matching documents. Phrase queries use the position lists to verify that terms appear consecutively.

# PageRank Computation
PageRank models the importance of a page based on the link structure. From the crawled pages, we build a directed graph where nodes are pages and edges are hyperlinks. The graph contains over 50,000 nodes and hundreds of thousands of edges.

We compute PageRank using the iterative power method with damping factor d = 0.85. The algorithm runs for 15 iterations until convergence (residual < 1e-6). Because the graph is large, we use sparse matrix representations (via SciPy) to perform the matrix‑vector multiplications efficiently. The resulting PageRank scores are stored in PostgreSQL and associated with each document. They are updated periodically after major crawls.

# Blended Ranking
The final relevance score for a document given a query is a linear combination of BM25 and PageRank:

score = α * BM25_score + (1 - α) * PageRank_score

We experimented with different α values on a validation set and found that α = 0.7 (70% BM25, 30% PageRank) gives the best balance between term‑based relevance and authority. This blend ensures that highly relevant pages are not overshadowed by authoritative but less relevant ones, while still boosting trusted domains.

To validate the improvement, we manually created a set of 50 diverse queries covering different topics and information needs. For each query, we compared the top 10 results from the blended ranking against a BM25‑only baseline. Blended ranking was judged superior (more relevant results at the top) in 38 out of 50 cases, confirming the value of incorporating link analysis.

# FastAPI Query Layer
The search API is built with FastAPI for high performance, automatic OpenAPI documentation, and async support. The main endpoints are:

GET /search?q=<query>&page=<page>&page_size=<size> – returns paginated search results.

GET /suggest?q=<prefix> – returns auto‑suggestions.

GET /document/<id> – retrieves a stored document.

When a query arrives, the API first checks Redis for a cached result (keyed by normalized query and page). If found, it returns immediately. Otherwise, it:

Parses the query using the parser.

Retrieves the list of matching documents from the inverted index (via SQL joins).

For each document, computes BM25 score and fetches its PageRank score.

Blends and sorts the results.

Caches the top results in Redis with a TTL (e.g., 10 minutes).

Returns the paginated response.

Performance: Under load, 85% of queries are served in under 100ms. The remaining 15% (uncached queries or complex phrase queries) take up to 300ms. The Redis cache hit rate is 63%, significantly reducing database load.

# Auto‑Suggest with Trie
To provide real‑time query suggestions as the user types, we built a prefix trie in memory. The trie is populated from all unique terms in the index (about 200,000 terms). When the user sends a prefix, we traverse the trie and collect the top completions (by term frequency). The entire operation takes less than 10ms, ensuring a smooth user experience. The trie is rebuilt periodically (e.g., after each crawl) and stored in a separate container; updates are done without downtime.

# Docker Deployment
All services are containerized. The docker-compose.yml defines:

postgres: database with persistent volume.

redis: in‑memory cache.

crawler: runs the crawler (can be scaled horizontally).

indexer: one‑off job to build/update the index.

pagerank: scheduled job to recompute PageRank.

api: FastAPI application, exposed on port 8000.

suggest: auto‑suggest service (can be scaled).

Environment variables (database URLs, cache settings, crawler concurrency) are managed through a .env file. This setup allows easy local development and production deployment.

# Evaluation and Results
The system was tested extensively:

Crawl throughput: 200+ pages/min, 50,000 pages indexed.

Index size: 50,000 documents, ~200,000 unique terms, millions of postings.

Query latency: 85% < 100ms, 15% < 300ms.

Cache hit rate: 63%.

Auto‑suggest latency: < 10ms.

Relevance improvement: 38/50 queries better than BM25 alone.

These numbers demonstrate that the system is both efficient and effective, capable of handling a substantial document collection with low latency.

# Future Improvements
Potential enhancements include:

Distributed indexing: Shard the index across multiple PostgreSQL instances or use Elasticsearch for scalability.

Real‑time updates: Support incremental index updates without full rebuild.

Learning to rank: Incorporate click‑through data to train a ranking model.

Spell correction: Add a spelling corrector for misspelled queries.

Query expansion: Use word embeddings to expand queries with related terms.

# Technology Stack
Language: Python 3.10+

Web Framework: FastAPI

Database: PostgreSQL (with pgvector for future ML)

Cache: Redis

Containerization: Docker, Docker Compose

Libraries: BeautifulSoup (crawling), SciPy (PageRank), ply (parsing), nltk (tokenization), SQLAlchemy (ORM)

# Conclusion
This distributed search engine showcases a complete implementation of core information retrieval and web search technologies. By blending BM25 with PageRank, we achieve superior relevance, and by leveraging modern tools like FastAPI, Redis, and Docker, we ensure high performance and scalability. The project demonstrates deep understanding of search algorithms, system design, and production‑ready coding practices – skills directly applicable to roles in backend engineering, data engineering, and search relevance. The code is clean, well‑documented, and easily runnable, making it a valuable portfolio piece.
