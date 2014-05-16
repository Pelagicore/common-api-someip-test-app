#pragma once

#include <map>
#include "glib.h"
#include "assert.h"

#include "CommonAPI/CommonAPI.h"


/**
 * This helper class provides an easy way to create CommonAPI stubs and proxies and integrate the dispatching of
 * incoming events into a glib main loop.
 */
class GLibCommonAPIFactory {

	class GDispatchWrapper : public GSource {
public:
		GDispatchWrapper(CommonAPI::DispatchSource* dispatchSource) :
			m_dispatchSource(dispatchSource) {
		}
		CommonAPI::DispatchSource* m_dispatchSource;
	};

	static gboolean gWatchDispatcher(GIOChannel* source, GIOCondition condition, gpointer userData) {
		CommonAPI::Watch* watch = static_cast<CommonAPI::Watch*>(userData);
		watch->dispatch(condition);
		return true;
	}

	static gboolean dispatchPrepare(GSource* source, gint* timeout) {
		int64_t eventTimeout;
		return static_cast<GDispatchWrapper*>(source)->m_dispatchSource->prepare(eventTimeout);
	}

	static gboolean dispatchCheck(GSource* source) {
		return static_cast<GDispatchWrapper*>(source)->m_dispatchSource->check();
	}

	static gboolean dispatchExecute(GSource* source, GSourceFunc callback, gpointer userData) {
		static_cast<GDispatchWrapper*>(source)->m_dispatchSource->dispatch();
		return true;
	}

	static gboolean gTimeoutDispatcher(void* userData) {
		return static_cast<CommonAPI::DispatchSource*>(userData)->dispatch();
	}

	static GSourceFuncs& getStandardGLibSourceCallbackFuncs() {
		static GSourceFuncs standardGLibSourceCallbackFuncs = {dispatchPrepare, dispatchCheck, dispatchExecute, nullptr};
		return standardGLibSourceCallbackFuncs;
	}

public:
	static constexpr const char* DBUS_SYSTEM_BUS = "dbus-system";
	static constexpr const char* DBUS_SESSION_BUS = "dbus-session";

	GLibCommonAPIFactory(const char* factoryName = nullptr) {
		m_factoryName = ( (factoryName != nullptr) ? factoryName : "" );

		m_defaultRuntime = CommonAPI::Runtime::load();
		m_mainLoopContext = m_defaultRuntime->getNewMainLoopContext();

		m_mainloopFactory = createMainLoopFactory(m_mainLoopContext, factoryName);
		m_servicePublisher = m_defaultRuntime->getServicePublisher();
		assert(m_servicePublisher.get() != nullptr);
	}

	std::shared_ptr<CommonAPI::Factory> createMainLoopFactory(std::shared_ptr<CommonAPI::MainLoopContext> mainLoopContext,
								  const char* factoryName = nullptr,
								  const char* runtimeName = nullptr) {

		std::shared_ptr<CommonAPI::Factory> factory;

		std::shared_ptr<CommonAPI::Runtime> runtime =
			(runtimeName == nullptr) ? CommonAPI::Runtime::load() : CommonAPI::Runtime::load(runtimeName);
		assert(runtime.get() != nullptr);

		doSubscriptions();

		if ( (factoryName == nullptr) || (strlen(factoryName) == 0) )
			factory = runtime->createFactory(mainLoopContext);
		else {
			std::string s = factoryName;
			factory = runtime->createFactory(mainLoopContext, s, false);
		}

		assert(factory.get() != nullptr);

		return factory;
	}

	virtual ~GLibCommonAPIFactory() {
	}

	/**
	 * Set the GMainContext to use in case the default (NULL) context is not appropriate
	 */
	void setMainContext(GMainContext* context) {
		m_mainContext = context;
	}

	/**
	 * Build a commonAPI address from the given DBUS parameters
	 */
	static std::string buildCommonAPIDBusAddress(const char* interfaceName, const char* nodeName, const char* busName) {
		std::string s = "local:";
		s += interfaceName;
		s += ",";
		s += nodeName;
		s += ":";
		s += busName;
		return s;
	}

	/**
	 * Build a commonAPI address from the given parameters
	 */
	template<class ProxyType>
	static std::string buildProxyCommonAPIAddress() {
		std::string s = "local:";
		s += ProxyType::getInterfaceId();
		s += ":";
		s += ProxyType::getInterfaceId();
		return s;
	}

	/**
	 * Build a commonAPI address from the given parameters
	 */
	template<class ServiceType>
	static std::string buildServiceCommonAPIAddress() {
		std::string s = "local:";
		s += ServiceType::StubAdapterType::getInterfaceId();
		s += ":";
		s += ServiceType::StubAdapterType::getInterfaceId();
		return s;
	}


	template<typename _Stub>
	bool registerService(const std::shared_ptr<_Stub>& stub,
			     const std::string& participantId,
			     const std::string& serviceName,
			     const std::string& domain) {
		auto o = m_servicePublisher.get();
		return o->registerService(stub, participantId, serviceName, domain, m_mainloopFactory);
	}

	template<typename _Stub>
	bool registerService(const std::shared_ptr<_Stub>& stub, const std::string& serviceAddress) {
		auto o = m_servicePublisher.get();
		return o->registerService(stub, serviceAddress, m_mainloopFactory);
	}

	template<typename _Stub>
	bool registerService(const std::shared_ptr<_Stub>& stub) {
		return registerService( stub, buildServiceCommonAPIAddress<_Stub>() );
	}


	template<template<typename ...> class _ProxyClass, typename ... _AttributeExtensions>
	std::shared_ptr<_ProxyClass<_AttributeExtensions ...> >
	buildProxy(const std::string& participantId,
		   const std::string& serviceName,
		   const std::string& domain) {
		return m_mainloopFactory->buildProxy<_ProxyClass>(participantId, serviceName, domain);
	}

	template<template<typename ...> class _ProxyClass, typename ... _AttributeExtensions>
	std::shared_ptr<_ProxyClass<_AttributeExtensions ...> >
	buildProxy(const std::string serviceAddress) {
		return m_mainloopFactory->buildProxy<_ProxyClass>(serviceAddress);
	}

	template<template<typename ...> class _ProxyClass, typename ... _AttributeExtensions>
	std::shared_ptr<_ProxyClass<_AttributeExtensions ...> >
	buildProxy() {

		auto proxy =
			m_mainloopFactory->buildProxy<_ProxyClass>( buildProxyCommonAPIAddress<_ProxyClass<_AttributeExtensions
													   ...> >() );

#ifdef COMMON_API_PATCH_RUNTIMES_DISCOVERY
		if (proxy.get() == nullptr) {
			for( auto runtimeName : CommonAPI::Runtime::getRuntimeNames() ) {
				auto runtime = CommonAPI::Runtime::load(runtimeName);
				if (runtime.get() != nullptr) {
					auto factory = createMainLoopFactory( m_mainLoopContext,
									      m_factoryName.c_str(), runtimeName.c_str() );
					proxy =
						factory->buildProxy<_ProxyClass>( buildProxyCommonAPIAddress<_ProxyClass<
														     _AttributeExtensions
														     ...> >() );
					auto p = proxy.get();

					printf("p = %d\n", (size_t)p);

					if (p != NULL)
						return proxy;
				}
			}
		}
#endif

		return proxy;
	}


	CommonAPI::ServicePublisher& getServicePublisher() {
		return *( m_servicePublisher.get() );
	}

	CommonAPI::Factory& getFactory() {
		return *( m_mainloopFactory.get() );
	}

	void doSubscriptions() {
		m_mainLoopContext->subscribeForTimeouts(
			std::bind(&GLibCommonAPIFactory::timeoutAddedCallback, this, std::placeholders::_1, std::placeholders::_2),
			std::bind(&GLibCommonAPIFactory::timeoutRemovedCallback, this, std::placeholders::_1) );

		m_mainLoopContext->subscribeForWatches(
			std::bind(&GLibCommonAPIFactory::watchAddedCallback, this, std::placeholders::_1, std::placeholders::_2),
			std::bind(&GLibCommonAPIFactory::watchRemovedCallback, this, std::placeholders::_1) );

		m_mainLoopContext->subscribeForWakeupEvents( std::bind(&GLibCommonAPIFactory::wakeupMain, this) );
	}

	void watchAddedCallback(CommonAPI::Watch* watch, const CommonAPI::DispatchPriority dispatchPriority) {
		const pollfd& fileDesc = watch->getAssociatedFileDescriptor();
		GIOChannel* ioChannel = g_io_channel_unix_new(fileDesc.fd);

		GSource* gWatch = g_io_create_watch( ioChannel, static_cast<GIOCondition>(fileDesc.events) );
		g_source_set_callback(gWatch, reinterpret_cast<GSourceFunc>(&gWatchDispatcher), watch, NULL);

		//		const auto& dependentSources = watch->getDependentDispatchSources();
		for ( auto& dependentSource : watch->getDependentDispatchSources() ) {
			GSource* gDispatchSource = g_source_new( &getStandardGLibSourceCallbackFuncs(), sizeof(GDispatchWrapper) );
			static_cast<GDispatchWrapper*>(gDispatchSource)->m_dispatchSource = dependentSource;

			g_source_add_child_source(gWatch, gDispatchSource);

			gSourceMappings.insert({dependentSource, gDispatchSource});
		}
		g_source_attach(gWatch, m_mainContext);
	}

	void watchRemovedCallback(CommonAPI::Watch* watch) {
		// TODO : implement
		//		assert(false);

		//		g_source_remove_by_user_data(watch);
		//
		//		if (dbusChannel_) {
		//			g_io_channel_unref(dbusChannel_);
		//			dbusChannel_ = NULL;
		//		}
		//
		//		const auto& dependentSources = watch->getDependentDispatchSources();
		//		for (auto dependentSourceIterator = dependentSources.begin(); dependentSourceIterator != dependentSources.end();
		//		     dependentSourceIterator++) {
		//			GSource* gDispatchSource = g_source_new( &standardGLibSourceCallbackFuncs, sizeof(GDispatchWrapper) );
		//			GSource* gSource = gSourceMappings.find(*dependentSourceIterator)->second;
		//			g_source_destroy(gSource);
		//			g_source_unref(gSource);
		//		}
	}

	void timeoutAddedCallback(CommonAPI::Timeout* commonApiTimeoutSource,
				  const CommonAPI::DispatchPriority dispatchPriority) {
		GSource* gTimeoutSource = g_timeout_source_new( commonApiTimeoutSource->getTimeoutInterval() );
		g_source_set_callback(gTimeoutSource, &GLibCommonAPIFactory::gTimeoutDispatcher, commonApiTimeoutSource, NULL);
		g_source_attach(gTimeoutSource, m_mainContext);
	}

	void timeoutRemovedCallback(CommonAPI::Timeout* timeout) {
		g_source_remove_by_user_data(timeout);
	}

	void wakeupMain() {
		g_main_context_wakeup(m_mainContext);
	}

private:
	std::shared_ptr<CommonAPI::Runtime> m_defaultRuntime;
	std::shared_ptr<CommonAPI::MainLoopContext> m_mainLoopContext;
	std::shared_ptr<CommonAPI::Factory> m_mainloopFactory;
	std::shared_ptr<CommonAPI::ServicePublisher> m_servicePublisher;

	std::map<CommonAPI::DispatchSource*, GSource*> gSourceMappings;

	GMainContext* m_mainContext = nullptr;

	std::string m_factoryName;

};
