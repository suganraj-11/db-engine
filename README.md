<h1>db-engine (Archived)</h1>

<blockquote>
<strong>Note:</strong> This repository is no longer maintained. Development has been moved to 
<a href="https://github.com/sugvn/columnar-db">sugvn/columnar-db</a>.<br>
This repo is preserved for reference purposes only.
</blockquote>

<hr>

<h2>Overview</h2>
<p>
A simple database engine written in <strong>C++</strong>, developed as an experimental project to understand the fundamentals of database internals.<br>
It focuses on implementing the core building blocks of a DBMS from scratch — including storage, indexing, and query processing.
</p>

<hr>

<h2>Objectives</h2>
<ul>
  <li>Explore low-level implementation of database systems.</li>
  <li>Experiment with different indexing structures and query execution approaches.</li>
  <li>Build a minimal, functional SQL-like engine without relying on third-party database libraries.</li>
</ul>

<hr>

<h2>Development Progress</h2>
<ul>
  <li><strong>Database &amp; Table Creation</strong> – Basic functionality for creating databases and tables.</li>
  <li><strong>Data Insertion</strong> – Insert records into tables with basic data validation.</li>
  <li><strong>Indexing (AVL Tree)</strong> – Implemented AVL tree–based indexing for efficient searches, with plans to transition to <strong>B+ Trees</strong>.</li>
  <li><strong>Query Parsing &amp; Execution</strong> – Simple regex-based parser for executing basic queries.</li>
  <li><strong>Unit Tests</strong> – Small test cases written to verify implemented features.</li>
</ul>

<hr>

<h2>Planned Improvements</h2>
<p>Although development has shifted to the successor project, the following features were planned:</p>
<ul>
  <li><strong>B+ Tree Indexing</strong> – Replace AVL trees with B+ trees for disk-friendly, high-performance indexing.</li>
  <li><strong>Advanced Query Parser</strong> – Move from regex-based parsing to a tokenized parser for more complex SQL queries.</li>
  <li><strong>Query Optimizer</strong> – Basic query optimization strategies for faster execution.</li>
  <li><strong>Persistent Storage</strong> – Improve on-disk storage format for larger datasets.</li>
  <li><strong>Support for Constraints</strong> – Implement primary key, unique, and not-null constraints.</li>
  <li><strong>Transaction Support</strong> – Add ACID-compliant transaction handling.</li>
  <li><strong>Joins and Aggregations</strong> – Support multi-table queries and aggregate functions.</li>
</ul>

<hr>

<h2>Successor Project</h2>
<p>
Further development continues in 
<strong><a href="https://github.com/sugvn/columnar-db">sugvn/columnar-db</a></strong>,<br>
a columnar-storage–based database engine with improved indexing and query performance.
</p>
