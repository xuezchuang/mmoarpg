// Copyright (C) RenZhai.2020.All Rights Reserved.


#include "SimpleOSSFunctionLibrary.h"
#include "SimpleOSSManage.h"
#include "Async/TaskGraphInterfaces.h"
#include "SimpleOSSMacro.h"
#include "Misc/ScopeLock.h"

FCriticalSection OSSMutex;

FString USimpleOSSFunctionLibrary::GetEndpoint()
{
	return SIMPLE_OSS.GetEndpoint();
}

void USimpleOSSFunctionLibrary::InitAccounts(const FString &InAccessKeyId, const FString &InAccessKeySecret, const FString &InEndpoint)
{
	SIMPLE_OSS.InitAccounts(InAccessKeyId, InAccessKeySecret, InEndpoint);
}

void USimpleOSSFunctionLibrary::InitConf(const FClientConfiguration &InConf)
{
	SIMPLE_OSS.InitConf(InConf);
}

bool USimpleOSSFunctionLibrary::DoesBucketExist(const FString &InBucketName)
{
	return SIMPLE_OSS.DoesBucketExist(InBucketName);
}

bool USimpleOSSFunctionLibrary::CreateBucket(const FString &InBucketName, EOSSStorageType OSSStorageType /*= EOSSStorageType::IA*/, EAcl Acl /*= EAcl::PUBLICREAD*/)
{
	return SIMPLE_OSS.CreateBucket(InBucketName, OSSStorageType, Acl);
}

bool USimpleOSSFunctionLibrary::ListBuckets(TArray<FString> &InBucketName)
{
	return SIMPLE_OSS.ListBuckets(InBucketName);
}

bool USimpleOSSFunctionLibrary::GetBucketsInfo(const FString &InBucketName, FBucketInfo &InBucketInfo)
{
	return SIMPLE_OSS.GetBucketsInfo(InBucketName, InBucketInfo);
}

FString USimpleOSSFunctionLibrary::GetBucketLocation(const FString &InBucketName)
{
	return SIMPLE_OSS.GetBucketLocation(InBucketName);
}

//int64 USimpleOSSFunctionLibrary::GetBucketsStorageCapacity(const FString &InBucketName)
//{
//	return SIMPLE_OSS.GetBucketsStorageCapacity(InBucketName);
//}

bool USimpleOSSFunctionLibrary::DeleteBucket(const FString &InBucketName)
{
	return SIMPLE_OSS.DeleteBucket(InBucketName);
}

bool USimpleOSSFunctionLibrary::SetBucketAcl(const FString &InBucketName, EAcl BucketAcl)
{
	return SIMPLE_OSS.SetBucketAcl(InBucketName, BucketAcl);
}

EAcl USimpleOSSFunctionLibrary::GetBucketAcl(const FString InBucketName)
{
	return SIMPLE_OSS.GetBucketAcl(InBucketName);
}

bool USimpleOSSFunctionLibrary::SetBucketReferer(const FString &InBucketName, const TArray<FString> &URLReferer, bool bAllowEmptyRefere /*= false*/)
{
	return SIMPLE_OSS.SetBucketReferer(InBucketName, URLReferer, bAllowEmptyRefere);
}

bool USimpleOSSFunctionLibrary::GetBucketReferer(const FString &InBucketName, TArray<FString> &BucketReferers)
{
	return SIMPLE_OSS.GetBucketReferer(InBucketName, BucketReferers);
}

bool USimpleOSSFunctionLibrary::DeleteBucketReferer(const FString &InBucketName, bool bAllowEmptyRefere /*= true*/)
{
	return SIMPLE_OSS.DeleteBucketReferer(InBucketName, bAllowEmptyRefere);
}

bool USimpleOSSFunctionLibrary::GetObjectMeta(const FString &InBucketName, const FString &InObjectName, FOSSObjectMeta &OSSObjectInfo)
{
	return SIMPLE_OSS.GetObjectMeta(InBucketName, InObjectName, OSSObjectInfo);
}

EAcl USimpleOSSFunctionLibrary::GetObjectAcl(const FString &InBucketName, const FString &InObjectName)
{
	return SIMPLE_OSS.GetObjectAcl(InBucketName, InObjectName);
}

bool USimpleOSSFunctionLibrary::SetObjectAcl(const FString &InBucketName, const FString &InObjectName, EAcl InAcl)
{
	return SIMPLE_OSS.SetObjectAcl(InBucketName, InObjectName, InAcl);
}

bool USimpleOSSFunctionLibrary::DoesObjectExist(const FString &InBucketName, const FString &InObjectName)
{
	return SIMPLE_OSS.DoesObjectExist(InBucketName, InObjectName);
}

bool USimpleOSSFunctionLibrary::ListObjects(const FString &InBucketName, TArray<FOSSObjectInfo> &ObjectName, int32 MaxNumber /*= 100*/)
{
	return SIMPLE_OSS.ListObjects(InBucketName, ObjectName, MaxNumber);
}

bool USimpleOSSFunctionLibrary::ListObjectsByFilePaths(const FString &InBucketName, const FString &ServerFileRelativePaths, TArray<FOSSObjectInfo> &ObjectName, int32 MaxNumber /*= 100*/)
{
	return SIMPLE_OSS.ListObjectsByPrefix(InBucketName, ServerFileRelativePaths,ObjectName, MaxNumber);
}

bool USimpleOSSFunctionLibrary::RecursiveListObjectsByFilePaths(const FString &InBucketName, const FString &ServerFileRelativePaths, TArray<FOSSObjectInfo> &ObjectName, int32 MaxNumber /*= 100*/)
{
	return SIMPLE_OSS.RecursiveListObjectsByPrefix(InBucketName, ServerFileRelativePaths, ObjectName, MaxNumber);
}
//
//bool USimpleOSSFunctionLibrary::SetObjectStorageType(const FString &InBucketName, const FString &InObjectName, EOSSStorageType OSSStorageType)
//{
//	return SIMPLE_OSS.SetObjectStorageType(InBucketName, InObjectName, OSSStorageType);
//}
//
//EOSSStorageType USimpleOSSFunctionLibrary::GetObjectStorageType(const FString &InBucketName, const FString &InObjectName)
//{
//	return SIMPLE_OSS.GetObjectStorageType(InBucketName, InObjectName);
//}

bool USimpleOSSFunctionLibrary::CreateSymlink(const FString &InBucketName, const FString &InObjectName, const FString &InLinkObjectName)
{
	return SIMPLE_OSS.CreateSymlink(InBucketName, InObjectName, InLinkObjectName);
}

FString USimpleOSSFunctionLibrary::GetSymlink(const FString &InBucketName, const FString &InLinkName)
{
	return SIMPLE_OSS.GetSymlink(InBucketName, InLinkName);
}

bool USimpleOSSFunctionLibrary::GetObjectToLocal(FString InBucketName, FString InObjectName, FString InLocalPaths, FRange Range)
{
	return SIMPLE_OSS.GetObjectToLocal(InBucketName, InObjectName, InLocalPaths, Range, nullptr);
}

bool USimpleOSSFunctionLibrary::GetObjectToLocalByURL(const FString& URL, const FString& InLocalPaths, FRange Range)
{
	return SIMPLE_OSS.GetObjectToLocalByURL(URL, InLocalPaths, Range);
}

bool USimpleOSSFunctionLibrary::GetImageObjectByResize(const FVector2D &Size, const FString &InBucketName, const FString &InObjectName, const FString &InFileNametoSave)
{
	return SIMPLE_OSS.GetImageObjectByResize(Size, InBucketName, InObjectName, InFileNametoSave);
}

bool USimpleOSSFunctionLibrary::GetImageObjectByCrop(const FVector4 &Size, int32 Ratio, const FString &InBucketName, const FString &InObjectName, const FString &InFileNametoSave)
{
	return SIMPLE_OSS.GetImageObjectByCrop(Size, Ratio,InBucketName, InObjectName, InFileNametoSave);
}

bool USimpleOSSFunctionLibrary::GetImageObjectBySharpen(const int32 Sharpen, const FString &InBucketName, const FString &InObjectName, const FString &InFileNametoSave)
{
	return SIMPLE_OSS.GetImageObjectBySharpen(Sharpen, InBucketName, InObjectName, InFileNametoSave);
}

bool USimpleOSSFunctionLibrary::GetImageObjectByWaterMark(const FString WaterMarkObjectName, const FString &InBucketName, const FString &InObjectName, const FString &InFileNametoSave)
{
	return SIMPLE_OSS.GetImageObjectByWaterMark(WaterMarkObjectName, InBucketName, InObjectName, InFileNametoSave);
}

bool USimpleOSSFunctionLibrary::GetImageObjectByRotate(const int32 Rotate, const FString &InBucketName, const FString &InObjectName, const FString &InFileNametoSave)
{
	return SIMPLE_OSS.GetImageObjectByRotate(Rotate, InBucketName, InObjectName, InFileNametoSave);
}

bool USimpleOSSFunctionLibrary::GetImageObjectByStyle(const FString OSSStyleName, const FString &InBucketName, const FString &InObjectName, const FString &InFileNametoSave)
{
	return SIMPLE_OSS.GetImageObjectByStyle(OSSStyleName, InBucketName, InObjectName, InFileNametoSave);
}

bool USimpleOSSFunctionLibrary::GetImageObjectByCascade(const TArray<FString> &CascadeCommand, const FString &InBucketName, const FString &InObjectName, const FString &InFileNametoSave)
{
	return SIMPLE_OSS.GetImageObjectByCascade(CascadeCommand, InBucketName, InObjectName, InFileNametoSave);
}

bool USimpleOSSFunctionLibrary::GetImageInfo(const FString &InBucketName, const FString &InObjectName, const FString &InFileNametoSave)
{
	return SIMPLE_OSS.GetImageInfo(InBucketName, InObjectName, InFileNametoSave);
}

bool USimpleOSSFunctionLibrary::AbortMultipartUpload(const FString &InBucketName, const FString &InObjectName, const FString &InUploadId)
{
	return SIMPLE_OSS.AbortMultipartUpload(InBucketName, InObjectName, InUploadId);
}

bool USimpleOSSFunctionLibrary::GeneratePresignedUrl(FString &URL, const FString &InBucketName, const FString &InObjectName)
{
	return SIMPLE_OSS.GeneratePresignedUrl(URL, InBucketName, InObjectName);
}

bool USimpleOSSFunctionLibrary::UploadPart(FString &InUploadId, const FString &InBucketName, const FString &InObjectName, const FString &InLocalPaths, int32 PartSize /*= 1024 * 1024 * 10*/, const TMap<FString, FString> &OSSMeta /*= TMap<FString, FString>()*/)
{
	if (PartSize == 0)
	{
		PartSize = 1024 * 1024 * 10;
	}
	return SIMPLE_OSS.UploadPart(InUploadId, InBucketName, InObjectName, InLocalPaths, PartSize,OSSMeta);
}

bool USimpleOSSFunctionLibrary::ResumableUploadObject(const FString &InBucketName, const FString &InObjectName, const FString &InUploadFilePath, int64 PartSize, const TMap<FString, FString> &OSSMeta /*= TMap<FString, FString>()*/)
{
	if (PartSize == 0)
	{
		PartSize = 1024 * 1024 * 10;
	}
	return SIMPLE_OSS.ResumableUploadObject(InBucketName, InObjectName, InUploadFilePath, PartSize, nullptr, OSSMeta);
}

//bool USimpleOSSFunctionLibrary::AppendObject(const FString &InBucketName, const FString &InLocalPaths, const FString &InObjectName /*=""*/)
//{
//	return SIMPLE_OSS.AppendObject(InBucketName,InLocalPaths,InObjectName);
//}

bool USimpleOSSFunctionLibrary::PutObject(const FString &InBucketName, const FString &InLocalPaths, const FString &InObjectName /*= FString()*/, const TMap<FString, FString> &OSSMeta /*= TMap<FString, FString>()*/)
{
	return SIMPLE_OSS.PutObject(InBucketName, InLocalPaths, InObjectName,nullptr,OSSMeta);
}

bool USimpleOSSFunctionLibrary::PutObjectByURL(const FString& URL, const FString& InLocalPaths, const TMap<FString, FString> &OSSMeta)
{
	return SIMPLE_OSS.PutObjectByURL(URL, InLocalPaths, OSSMeta);
}

bool USimpleOSSFunctionLibrary::PutObjectByMemory(const FString &InBucketName, const FString &InObjectName, const FString &Data, const TMap<FString, FString> &OSSMeta)
{
	return SIMPLE_OSS.PutObjectByMemory(InBucketName,InObjectName, Data,nullptr, OSSMeta);
}

bool USimpleOSSFunctionLibrary::PutObjectMemoryByURL(const FString& URL, const FString &Data, const TMap<FString, FString> &OSSMeta)
{
	return SIMPLE_OSS.PutObjectMemoryByURL(URL, Data, OSSMeta);
}

bool USimpleOSSFunctionLibrary::GetObjectToMemory(const FString &InBucketName, const FString &InObjectName, FString &ContextString, FRange Range /*= FRange()*/)
{
	return SIMPLE_OSS.GetObjectToMemory(InBucketName, InObjectName, ContextString, Range, nullptr);
}

bool USimpleOSSFunctionLibrary::GetObjectToMemoryByURL(const FString& URL,FString &ContextString, FRange Range)
{
	return SIMPLE_OSS.GetObjectToMemoryByURL(URL,ContextString,Range);
}

bool USimpleOSSFunctionLibrary::ResumableDownloadObject(const FString &InBucketName, const FString &InObjectName, const FString &InDownloadFilePath, int32 PartSize /*= 1024 * 1024 * 10*/)
{
	if (PartSize == 0)
	{
		PartSize = 1024 * 1024 * 10;
	}

	return SIMPLE_OSS.ResumableDownloadObject(InBucketName, InObjectName, InDownloadFilePath, PartSize, NULL);
}

bool USimpleOSSFunctionLibrary::CopyObject(const FString &InSourceBucketName, const FString &InSourceObjectName, const FString &InCopyBucketName, const FString &InCopyObjectName)
{
	return SIMPLE_OSS.CopyObject(InSourceBucketName, InSourceObjectName, InCopyBucketName, InCopyObjectName);
}
//
//bool USimpleOSSFunctionLibrary::CopyPart(FString &UploadID, const FString &InSourceBucketName, const FString &InSourceObjectName, const FString &InCopyBucketName, const FString &InCopyObjectName)
//{
//	return SIMPLE_OSS.CopyPart(UploadID, InSourceBucketName, InSourceObjectName, InCopyBucketName, InCopyObjectName);
//}

bool USimpleOSSFunctionLibrary::DeleteObject(const FString &InBucketName, const FString &InObjectName)
{
	return SIMPLE_OSS.DeleteObject(InBucketName, InObjectName);
}

bool USimpleOSSFunctionLibrary::DeleteObjectByPrefix(const FString &InBucketName, const FString &InPrefix)
{
	return SIMPLE_OSS.DeleteObjectByPrefix(InBucketName, InPrefix);
}

bool USimpleOSSFunctionLibrary::DeleteObjectByFilePath(const FString &InBucketName, const FString &InServerFilePath)
{
	return SIMPLE_OSS.DeleteObjectByPrefix(InBucketName, InServerFilePath);
}

bool USimpleOSSFunctionLibrary::DeleteObjects(const FString &InBucketName, const TArray<FString> &InObjectName)
{
	return SIMPLE_OSS.DeleteObjects(InBucketName, InObjectName);
}

bool USimpleOSSFunctionLibrary::ListMultipartUploads(const FString &InBucketName, TArray<FMultipartPartsUploadsInfo> &MultipartPartsInfo, int32 MaxParts /*= 1000*/)
{
	return SIMPLE_OSS.ListMultipartUploads(InBucketName, MultipartPartsInfo, MaxParts);
}

DECLARATION_CALLBACK_DELEGATE(AsyncGetObjectToLocal)
void USimpleOSSFunctionLibrary::AsyncGetObjectToLocal(FString InBucketName, FString InObjectName, FString InLocalPaths, FRange Range, FProgressCallbackDelegate InProgressCallback)
{
	AsyncGetObjectToLocal_Delegate = InProgressCallback;
	SIMPLE_OSS.AsyncGetObjectToLocal(InBucketName, InObjectName, InLocalPaths, Range, AsyncGetObjectToLocal_Local);
}

DECLARATION_CALLBACK_DELEGATE(AsyncGetObjectToLocalByURL)
DECLARATION_END_CALLBACK_DELEGATE(AsyncGetObjectToLocalByURL)
DECLARATION_START_CALLBACK_DELEGATE(AsyncGetObjectToLocalByURL)
void USimpleOSSFunctionLibrary::AsyncGetObjectToLocalByURL(const FString& URL, const FString& InLocalPaths, FRange Range, FProgressCallbackDelegate InProgressCallback, FBufferByOSSCallbackDelegate InStartCallback, FBufferByOSSCallbackDelegate InEndCallback)
{
	AsyncGetObjectToLocalByURL_Delegate = InProgressCallback;
	AsyncGetObjectToLocalByURL_Start_Delegate = InStartCallback;
	AsyncGetObjectToLocalByURL_End_Delegate = InEndCallback;
	SIMPLE_OSS.AsyncGetObjectToLocalByURL(URL, InLocalPaths, Range, AsyncGetObjectToLocalByURL_Local, AsyncGetObjectToLocalByURL__Start_Delegate,AsyncGetObjectToLocalByURL__End_Delegate);
}

DECLARATION_CALLBACK_DELEGATE(AsyncGetObjectToMemory)
DECLARATION_END_CALLBACK_DELEGATE(AsyncGetObjectToMemory)
DECLARATION_START_CALLBACK_DELEGATE(AsyncGetObjectToMemory)
void USimpleOSSFunctionLibrary::AsyncGetObjectToMemory(const FString &InBucketName, const FString &InObjectName, FRange Range, FProgressCallbackDelegate InProgressCallback, FBufferByOSSCallbackDelegate InStartCallback, FBufferByOSSCallbackDelegate InEndCallback)
{
	AsyncGetObjectToMemory_Delegate = InProgressCallback;
	AsyncGetObjectToMemory_Start_Delegate = InStartCallback;
	AsyncGetObjectToMemory_End_Delegate = InEndCallback;
	SIMPLE_OSS.AsyncGetObjectToMemory(InBucketName, InObjectName,Range, AsyncGetObjectToMemory_Local,AsyncGetObjectToMemory__Start_Delegate,AsyncGetObjectToMemory__End_Delegate);
}

DECLARATION_CALLBACK_DELEGATE(AsyncGetObjectToMemoryByURL)
DECLARATION_END_CALLBACK_DELEGATE(AsyncGetObjectToMemoryByURL)
DECLARATION_START_CALLBACK_DELEGATE(AsyncGetObjectToMemoryByURL)
void USimpleOSSFunctionLibrary::AsyncGetObjectToMemoryByURL(
	const FString& URL, 
	FRange Range, 
	FProgressCallbackDelegate InProgressCallback,
	FBufferByOSSCallbackDelegate InStartCallback,
	FBufferByOSSCallbackDelegate InEndCallback)
{
	AsyncGetObjectToMemoryByURL_Delegate = InProgressCallback;
	AsyncGetObjectToMemoryByURL_End_Delegate = InEndCallback;
	AsyncGetObjectToMemoryByURL_Start_Delegate = InStartCallback;
	SIMPLE_OSS.AsyncGetObjectToMemoryByURL(URL, Range, AsyncGetObjectToMemoryByURL_Local, AsyncGetObjectToMemoryByURL__Start_Delegate, AsyncGetObjectToMemoryByURL__End_Delegate);
}

DECLARATION_CALLBACK_DELEGATE(AsyncResumableDownloadObject)
void USimpleOSSFunctionLibrary::AsyncResumableDownloadObject(const FString &InBucketName, const FString &InObjectName, const FString &InDownloadFilePath, int32 PartSize,FProgressCallbackDelegate InProgressCallback)
{
	if (PartSize == 0)
	{
		PartSize = 1024 * 10;
	}
	AsyncResumableDownloadObject_Delegate = InProgressCallback;
	SIMPLE_OSS.AsyncResumableDownloadObject(InBucketName, InObjectName, InDownloadFilePath, PartSize, AsyncResumableDownloadObject_Local);
}

DECLARATION_CALLBACK_DELEGATE_PART(AsyncUploadPart)
FString USimpleOSSFunctionLibrary::AsyncUploadPart(const FString &InBucketName, const FString &InObjectName, const FString &InLocalPaths, int32 PartSize, FCallbackUploadPartDelegate InProgressCallback, const TMap<FString, FString> &OSSMeta)
{
	if (PartSize == 0)
	{
		PartSize = 1024 * 1024 * 10;
	}
	AsyncUploadPart_Delegate = InProgressCallback;
	return SIMPLE_OSS.AsyncUploadPart(InBucketName, InObjectName, InLocalPaths, PartSize, AsyncUploadPart_Local, OSSMeta);
}

DECLARATION_CALLBACK_DELEGATE(AsyncPutObject)
void USimpleOSSFunctionLibrary::AsyncPutObject(const FString &InBucketName,  const FString &InObjectName /*= FString()*/,const FString &InLocalPaths, FProgressCallbackDelegate InProgressCallback /*= nullptr*/, const TMap<FString, FString> &OSSMeta /*= TMap<FString, FString>()*/)
{
	AsyncPutObject_Delegate = InProgressCallback;
	SIMPLE_OSS.AsyncPutObject(InBucketName, InLocalPaths,InObjectName, AsyncPutObject_Local, OSSMeta);
}

DECLARATION_CALLBACK_DELEGATE(AsyncPutObjectByURL)
void USimpleOSSFunctionLibrary::AsyncPutObjectByURL(const FString& URL, const FString& InLocalPaths, FProgressCallbackDelegate InProgressCallback, const TMap<FString, FString> &OSSMeta)
{
	AsyncPutObjectByURL_Delegate = InProgressCallback;
	SIMPLE_OSS.AsyncPutObjectMemoryByURL(URL, InLocalPaths, OSSMeta, AsyncPutObjectByURL_Local);
}

DECLARATION_CALLBACK_DELEGATE(AsyncPutObjectByMemory)
void USimpleOSSFunctionLibrary::AsyncPutObjectByMemory(const FString &InBucketName, const FString &InObjectName, const FString &Data, FProgressCallbackDelegate InProgressCallback, const TMap<FString, FString> &OSSMeta)
{
	AsyncPutObjectByMemory_Delegate = InProgressCallback;
	SIMPLE_OSS.AsyncPutObjectByMemory(InBucketName, InObjectName, Data,AsyncPutObjectByMemory_Local, OSSMeta);
}

DECLARATION_CALLBACK_DELEGATE(AsyncPutObjectMemoryByURL)
void USimpleOSSFunctionLibrary::AsyncPutObjectMemoryByURL(const FString& URL, const FString &Data, FProgressCallbackDelegate InProgressCallback, const TMap<FString, FString> &OSSMeta)
{
	AsyncPutObjectMemoryByURL_Delegate = InProgressCallback;
	SIMPLE_OSS.AsyncPutObjectMemoryByURL(URL, Data,OSSMeta,AsyncPutObjectMemoryByURL_Local);
}

DECLARATION_CALLBACK_DELEGATE(AsyncResumableUploadObject)
void USimpleOSSFunctionLibrary::AsyncResumableUploadObject(const FString &InBucketName, const FString &InObjectName, const FString &InUploadFilePath, int64 PartSize, FProgressCallbackDelegate InProgressCallback /*= nullptr*/, const TMap<FString, FString> &OSSMeta /*= TMap<FString, FString>()*/)
{
	if (PartSize == 0)
	{
		PartSize = 1024 * 1024 * 10;
	}
	AsyncResumableUploadObject_Delegate = InProgressCallback;
	SIMPLE_OSS.AsyncResumableUploadObject(InBucketName, InObjectName, InUploadFilePath, PartSize,AsyncResumableUploadObject_Local, OSSMeta);
}
