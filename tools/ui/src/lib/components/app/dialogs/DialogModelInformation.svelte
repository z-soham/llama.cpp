<script lang="ts">
	import ModelsManagerModelConfigurationInformation from '$lib/components/app/models/ModelsManager/ModelsManagerModelConfiguration/ModelsManagerModelConfigurationInformation.svelte';
	import * as Dialog from '$lib/components/ui/dialog';
	import { MODEL_ICON } from '$lib/constants';
	import { HuggingFaceService } from '$lib/services';
	import { modelsStore, uiStore } from '$lib/stores';
	import type { HfModelDetailInfo } from '$lib/types/huggingface';
	import { repoOf } from '$lib/utils';

	// the props cache is a plain Map, so the read has to name its version to stay reactive
	let serverProps = $derived.by(() => {
		void modelsStore.props.cacheVersion;

		return uiStore.modelInformation
			? modelsStore.props.getModelProps(uiStore.modelInformation.model)
			: null;
	});

	// The server reports the full metadata only once a model is loaded. With the
	// Hub enabled, it fills those gaps instead.
	let hubDetails = $state<HfModelDetailInfo | null>(null);

	$effect(() => {
		const option = uiStore.modelInformation;

		if (!option) return;

		const repo = repoOf(option.model);

		let cancelled = false;

		hubDetails = null;

		if (!HuggingFaceService.isEnabled() || !repo.includes('/')) {
			return;
		}

		void HuggingFaceService.getDetails(repo)
			.then((details) => {
				if (!cancelled) hubDetails = details;
			})
			.catch(() => {});

		return () => {
			cancelled = true;
		};
	});
</script>

<Dialog.Root
	onOpenChange={(open) => {
		if (!open) uiStore.modelInformation = null;
	}}
	open={uiStore.modelInformation !== null}
>
	<Dialog.Content
		class="max-h-[85dvh] w-[calc(100vw-2rem)]! max-w-md!"
		onOpenAutoFocus={(event) => event.preventDefault()}
	>
		<Dialog.Header class="flex flex-row items-center pr-8">
			<Dialog.Title class="flex min-w-0 items-center gap-2 text-base">
				<MODEL_ICON class="h-4 w-4 shrink-0 text-muted-foreground" />

				{#if uiStore.modelInformation}
					<span class="min-w-0 truncate">{uiStore.modelInformation.model}</span>
				{/if}
			</Dialog.Title>

			<Dialog.Description class="sr-only">
				Details of the model, from the server or the Hub.
			</Dialog.Description>
		</Dialog.Header>

		<div class="min-h-0 overflow-y-auto pb-4">
			{#if uiStore.modelInformation}
				<ModelsManagerModelConfigurationInformation
					hub={hubDetails}
					option={uiStore.modelInformation}
					{serverProps}
				/>
			{/if}
		</div>
	</Dialog.Content>
</Dialog.Root>
