# Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      https://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import typing as tp

from infinity import URI
from infinity.remote_thrift.infinity_thrift_rpc import *
from infinity.remote_thrift.infinity_thrift_rpc.ttypes import *
from thrift import Thrift
from thrift.transport import TSocket
from thrift.transport import TTransport
from thrift.protocol import TBinaryProtocol

class ThriftInfinityClient:
    def __init__(self, uri: URI):
        match uri.port:
            case 9070:
                self.transport = TTransport.TFramedTransport(TSocket.TSocket(uri.ip, uri.port)) # async
            case _:
                self.transport = TTransport.TBufferedTransport(TSocket.TSocket(uri.ip, uri.port)) # sync
        self.protocol = TBinaryProtocol.TBinaryProtocol(self.transport)
        self.client = InfinityService.Client(self.protocol)
        self.transport.open()
        res = self.client.Connect()
        self.session_id = res.session_id

    def create_database(self, db_name: str):
        return self.client.CreateDatabase(CreateDatabaseRequest(session_id=self.session_id,
                                                                db_name=db_name,
                                                                option=None))

    def drop_database(self, db_name: str):
        return self.client.DropDatabase(DropDatabaseRequest(session_id=self.session_id,
                                                            db_name=db_name))

    def list_databases(self):
        return self.client.ListDatabase(ListDatabaseRequest(session_id=self.session_id))

    def describe_database(self, db_name: str):
        return self.client.DescribeDatabase(DescribeDatabaseRequest(session_id=self.session_id,
                                                                    db_name=db_name))

    def get_database(self, db_name: str):
        return self.client.GetDatabase(GetDatabaseRequest(session_id=self.session_id,
                                                          db_name=db_name))

    def create_table(self, db_name: str, table_name: str, column_defs, option):
        return self.client.CreateTable(CreateTableRequest(session_id=self.session_id,
                                                          db_name=db_name,
                                                          table_name=table_name,
                                                          column_defs=column_defs,
                                                          option=option))

    def drop_table(self, db_name: str, table_name: str):
        return self.client.DropTable(DropTableRequest(session_id=self.session_id,
                                                      db_name=db_name,
                                                      table_name=table_name))

    def list_tables(self, db_name: str):
        return self.client.ListTable(ListTableRequest(session_id=self.session_id,
                                                      db_name=db_name))

    def describe_table(self, db_name: str, table_name: str):
        return self.client.DescribeTable(DescribeTableRequest(session_id=self.session_id,
                                                              db_name=db_name,
                                                              table_name=table_name))

    def get_table(self, db_name: str, table_name: str):
        return self.client.GetTable(GetTableRequest(session_id=self.session_id,
                                                    db_name=db_name,
                                                    table_name=table_name))

    def create_index(self, db_name: str, table_name: str, index_name: str, index_info_list, option):
        return self.client.CreateIndex(CreateIndexRequest(session_id=self.session_id,
                                                          db_name=db_name,
                                                          table_name=table_name,
                                                          index_name=index_name,
                                                          index_info_list=index_info_list,
                                                          option=option))

    def drop_index(self, db_name: str, table_name: str, index_name: str):
        return self.client.DropIndex(DropIndexRequest(session_id=self.session_id,
                                                      db_name=db_name,
                                                      table_name=table_name,
                                                      index_name=index_name))

    def insert(self, db_name: str, table_name: str, column_names: list[str], fields: list[Field]):
        return self.client.Insert(InsertRequest(session_id=self.session_id,
                                                db_name=db_name,
                                                table_name=table_name,
                                                column_names=column_names,
                                                fields=fields))

    def import_data(self, db_name: str, table_name: str, file_path: str, import_options):
        return self.client.Import(ImportRequest(session_id=self.session_id,
                                                db_name=db_name,
                                                table_name=table_name,
                                                file_path=file_path,
                                                import_option=import_options))

    def select(self, db_name: str, table_name: str, select_list, where_expr, group_by_list, limit_expr, offset_expr,
               search_expr):
        return self.client.Select(SelectRequest(session_id=self.session_id,
                                                db_name=db_name,
                                                table_name=table_name,
                                                select_list=select_list,
                                                where_expr=where_expr,
                                                group_by_list=group_by_list,
                                                limit_expr=limit_expr,
                                                offset_expr=offset_expr,
                                                ))

    def disconnect(self):
        res = self.client.Disconnect(CommonRequest(session_id=self.session_id))
        self.transport.close()
        return res